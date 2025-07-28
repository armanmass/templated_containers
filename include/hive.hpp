#include <cstddef>
#include <iostream>
#include <type_traits>
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class hive
{
private:
    struct Element;
    struct Block;
    struct BlockDeleter;

    // Allocator for T objects inside elements inside blocks
    using AllocTraits = std::allocator_traits<Allocator>;

    // Allocator for blocks
    using BlockAllocator = typename AllocTraits::template rebind_alloc<Block>;
    using BlockAllocTraits = std::allocator_traits<BlockAllocator>;

    //Allocator for elements inside blocks
    using ElementAllocator = typename AllocTraits::template rebind_alloc<Element>;
    using ElementAllocTraits = std::allocator_traits<ElementAllocator>;

    // Elements encapsulate data
    struct Element
    {
        size_t skip{ 1 };

        Block* parent{ nullptr };
        
        // next free is for the free list, either holding data or on free list if erased
        union
        {
            T data;
            Element* next_free_{ nullptr };
        };

        Element()
            : next_free_(nullptr)
        { }

        ~Element() {}
    };

    // Blocks encapsulate elements
    struct Block
    {
        size_t   capacity_{ };
        Element* elements_{ nullptr };

        std::unique_ptr<Block, BlockDeleter> next;
        Block* prev{ nullptr };

        size_t active_count_{ };
        size_t highest_untouched_{ };
    };

    // Delete each element inside the block then the block
    struct BlockDeleter
    {
        BlockAllocator block_alloc_;

        explicit BlockDeleter(const BlockAllocator& balloc_ = BlockAllocator()) noexcept
            : block_alloc_(balloc_)
        { }

        void operator()(Block* block)
        {
            if (block == nullptr) return;
            ElementAllocator element_alloc_(block_alloc_);
            ElementAllocTraits::deallocate(element_alloc_, block->elements_, block->capacity_);

            BlockAllocTraits::destroy(block_alloc_, block);
            BlockAllocTraits::deallocate(block_alloc_, block,1);
        }
    };

    /* --- Base Iterator Class --- */
public:

    template<bool Const>
    class base_iterator
    {
    public:
        using pointer   = std::conditional_t<Const, const T*, T*>;
        using reference = std::conditional_t<Const, const T&, T&>;

        base_iterator() = default;
        operator base_iterator<true>() const
        {
            return base_iterator<true>(current_block_, idx_in_block_);
        }

        [[nodiscard]] reference operator*() const
        {
            return this->current_block_->elements_[idx_in_block_].data;
        }

        [[nodiscard]] pointer operator->() const
        {
            return &this->current_block_->elements_[idx_in_block_].data;
        }

        // Recursive crash occurs in clanged when not using 'this'
        // Not sure why (maybe due to const template resolution?)
        base_iterator& operator++()
        {
            if (this->current_block_ == nullptr) 
                return *this;

            ++this->idx_in_block_;

            while (this->current_block_ != nullptr)
            {
                while (this->idx_in_block_ < this->current_block_->highest_untouched_)
                {
                    if (this->current_block_->elements_[idx_in_block_].skip == 0)
                        return *this;

                    this->idx_in_block_ += this->current_block_->elements_[idx_in_block_].skip;
                }
                this->current_block_ = this->current_block_->next.get();
                this->idx_in_block_ = 0;
            }
            return *this;
        }

        base_iterator operator++(int)
        {
            base_iterator temp = *this;
            ++(*this);
            return temp;
        }

        [[nodiscard]] bool operator==(const base_iterator& other) const
        {
            return current_block_ == other.current_block_ && idx_in_block_ == other.idx_in_block_;
        }
    
    private:
        friend class hive;
        using BlockPtr = std::conditional_t<Const, const Block*, Block*>;
        BlockPtr current_block_{ nullptr };
        size_t idx_in_block_{ };

        base_iterator(BlockPtr block, size_t idx)
            : current_block_(block),
              idx_in_block_(idx)
        { }
    };

    using iterator       = base_iterator<false>;
    using const_iterator = base_iterator<true>;


    /* --- Member Variables --- */
private:
    using BlockPtr = std::unique_ptr<Block, BlockDeleter>;
    BlockPtr first_block_;
    Block*   last_block_{ nullptr };

    Element*  free_list_head_{ nullptr };
    Allocator allocator_{ };

    size_t size_{ };
    size_t capacity_{ };
    static constexpr size_t INITIAL_CAPACITY{ 4 };
    size_t next_block_capacity_{ INITIAL_CAPACITY };


    /* --- Hive Special Member Functions --- */
public:
    explicit hive(const Allocator& alloc = Allocator())
        : allocator_(alloc)
    { }

    ~hive() { clear(); }

    void clear() noexcept
    {
        if (first_block_ == nullptr) return;

        Block* curr_block = first_block_.get();
        while (curr_block != nullptr)
        {
            for (size_t i{}; i<curr_block->highest_untouched_; ++i)
            {
                if (curr_block->elements_[i].skip == 0)
                    AllocTraits::destroy(allocator_, &curr_block->elements_[i].data);
            }

            curr_block = curr_block->next.get();
        }

        first_block_.reset(); // next pointer is unique so recursive destruct? (i hope)
        last_block_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

    void swap(hive& other) noexcept
    {
        using std::swap;

        swap(first_block_, other.first_block_);
        swap(last_block_ , other.last_block_);

        swap(free_list_head_, other.free_list_head_);
        swap(allocator_, other.allocator_);

        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
    }

    [[nodiscard]] bool is_empty() const noexcept { return size_ == 0; }
    [[nodiscard]] size_t size() const noexcept { return size_; }
    [[nodiscard]] size_t capacity() const noexcept { return capacity_; }

    [[nodiscard]] iterator begin() noexcept;
    [[nodiscard]] const_iterator begin() const noexcept;

    [[nodiscard]] iterator end() noexcept { return iterator(nullptr, 0); }
    [[nodiscard]] const_iterator end() const noexcept { return const_iterator(nullptr, 0); }

    // TODO: implement insert
    iterator insert(const T& obj);
    iterator insert(T&& obj);
    
    template<typename... Args>
    iterator emplace(Args&&... args);

    iterator erase(iterator itr);

private:
    // TODO; implement insert internal
    template<typename U>
    iterator insert_internal(U&& obj);

    void add_block();
    void update_skipfield_on_emplace(Block* block, size_t idx);
    void update_skipfield_on_erase(Block* block, size_t idx);
};

    /* --- Thick Functions --- */

template<typename T, typename Allocator>
void hive<T, Allocator>::add_block()
{
    BlockAllocator block_alloc{ allocator_ };
    ElementAllocator elem_alloc{ allocator_ };

    Block* raw_block{ BlockAllocTraits::allocate(block_alloc, 1) };
    BlockAllocTraits::construct(block_alloc, raw_block);


    BlockPtr new_block{ raw_block, BlockDeleter{ block_alloc }};
    new_block->capacity_ = next_block_capacity_;
    new_block->elements_ = ElementAllocTraits::allocate(elem_alloc, next_block_capacity_);

    for (size_t i{}; i < new_block->capacity_; ++i)
        ElementAllocTraits::construct(elem_alloc, &new_block->elements_[i]);

    this->capacity_ += next_block_capacity_;

    if (last_block_ == nullptr) [[unlikely]]
    {
        first_block_ = std::move(new_block);
        last_block_ = first_block_.get();
    }
    else
    {
        new_block->prev = last_block_;
        last_block_->next = std::move(new_block);
        last_block_ = last_block_->next.get();
    }

    for (size_t i{}; i<next_block_capacity_; ++i)
        last_block_->elements_[i].parent = last_block_;

    last_block_->elements_[0].skip = last_block_->capacity_;
    last_block_->elements_[next_block_capacity_-1].skip = last_block_->capacity_;

    next_block_capacity_ *= 2;
}

template<typename T, typename Allocator>
typename hive<T, Allocator>::iterator 
hive<T,Allocator>::begin() noexcept
{    
    if (last_block_ == nullptr || is_empty()) 
        return end();

    Block* curr_block = first_block_.get();

    while (curr_block != nullptr)
    {
        for (size_t idx{}; idx<curr_block->highest_untouched_; ++idx)
        {
            if (curr_block->elements_[idx].skip == 0)
                return iterator(curr_block, idx);
        }
        curr_block = curr_block->next.get();
    }

    return end();
}

template<typename T, typename Allocator>
typename hive<T, Allocator>::const_iterator
hive<T,Allocator>::begin() const noexcept
{    
    if (last_block_ == nullptr || is_empty()) 
        return end();

    Block* curr_block = first_block_.get();

    while (curr_block != nullptr)
    {
        for (size_t idx{}; idx<curr_block->highest_untouched_; ++idx)
        {
            if (curr_block->elements_[idx].skip == 0)
                return const_iterator(curr_block, idx);
        }
        curr_block = curr_block->next.get();
    }

    return end();
}

template<typename T, typename Allocator>
template<typename... Args>
typename hive<T, Allocator>::iterator 
hive<T, Allocator>::emplace(Args&&... args)
{
    Block* free_parent{ nullptr };
    Element* free_element{ nullptr };
    size_t free_idx{ };

    if (free_list_head_ != nullptr)
    {
        free_element = free_list_head_;
        free_list_head_ = free_list_head_->next_free_;

        free_parent = free_element->parent;
        free_idx = free_element - free_parent->elements_;
    }
    else
    {
        if (last_block_ == nullptr || last_block_->active_count_ == last_block_->capacity_)
        {
            add_block();
        }
std::cout << "Num active before insert: " << last_block_->active_count_ << " Block capacity: " << last_block_->capacity_ << '\n';
        free_parent = last_block_;

        free_idx = free_parent->highest_untouched_;
        free_element = &free_parent->elements_[free_idx];
        ++free_parent->highest_untouched_;
    }

    //construct element within block
    AllocTraits::construct(allocator_, &free_element->data, std::forward<Args>(args)...);

    ++free_parent->active_count_;
    ++size_;

    update_skipfield_on_emplace(free_parent, free_idx);
std::cout << "Inserted " << free_element->data << " at idx: " << free_idx << '\n';
    return iterator(free_parent, free_idx);
}

template<typename T, typename Allocator>
void hive<T, Allocator>::update_skipfield_on_emplace(Block* block, size_t idx)
{
   Element& new_element = block->elements_[idx]; 
   size_t old_skip = new_element.skip;
   new_element.skip = 0;

   if (old_skip > 1) [[likely]]
   {
        --old_skip;
        if (idx+1 < block->capacity_) [[likely]]
            block->elements_[idx+1].skip = old_skip;
        block->elements_[idx+old_skip].skip = old_skip;
   }
}

template<typename T, typename Allocator>
typename hive<T, Allocator>::iterator
hive<T, Allocator>::erase(iterator itr)
{
    if (itr.current_block_ == nullptr || 
        itr.current_block_->elements_[itr.idx_in_block_].skip > 0)
    {
        return end();
    }

    auto block = itr.current_block_;
    const size_t idx = itr.idx_in_block_;
    Element& element_to_erase = block->elements_[idx];

    AllocTraits::destroy(allocator_, &element_to_erase.data);

    element_to_erase.next_free_ = free_list_head_;
    free_list_head_ = &element_to_erase;

    update_skipfield_on_erase(block, idx);

    --size_;
    --block->active_count_;

    // consider shrinking highest_untouched if fully connects

    ++itr;
    return itr;
}

template<typename T, typename Allocator>
void hive<T, Allocator>::update_skipfield_on_erase(Block* block, size_t idx)
{
    size_t left_gap{ };
    size_t right_gap{ };

    // if next block is active (skip == 0) then no change else coalesce
    if (idx+1 < block->highest_untouched_)
        right_gap = block->elements_[idx+1].skip;

    // if empty element to our left coalesce with it as well
    if (idx>0)
        left_gap = block->elements_[idx-1].skip;

    //update idx-left_gap+1 to left_gap+right_gap+1
    //update idx+right_gap-1 to left_gap+right_gap+1
    const size_t new_gap = left_gap + 1 + right_gap;

    block->elements_[idx-left_gap].skip = new_gap;
    block->elements_[idx+right_gap].skip = new_gap;
}