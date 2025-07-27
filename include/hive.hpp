#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

template <typename T, typename Allocator = std::allocator<T>>
class hive
{
private:
    struct Element;
    struct Block;
    struct BlockDeleter;

    // allocator for T objects inside elements inside blocks
    using AllocTraits = std::allocator_traits<Allocator>;

    // allocator for blocks
    using BlockAllocator = typename AllocTraits::template rebind_alloc<Block>;
    using BlockAllocTraits = std::allocator_traits<BlockAllocator>;

    //allocator for elements inside blocks
    using ElementAllocator = typename AllocTraits::template rebind_alloc<Element>;
    using ElementAllocTraits = std::allocator_traits<ElementAllocator>;

    // elements encapsulate data
    struct Element
    {
        enum class State { Active, Erased };

        State state_{ State::Erased };
        size_t skip{ };

        union
        {
            T data;
            Element* next_free_{ nullptr };
        };
    };

    // blocks encapsulate elements
    struct Block
    {
        size_t   capacity_{ };
        Element* elements_{ nullptr };

        std::unique_ptr<Block, BlockDeleter> next{ nullptr };
        Block* prev{ nullptr };

        size_t active_count_{ };
        size_t highest_untouched_{ };
    };

    // delete each elemnt inside the block then the block
    struct BlockDeleter
    {
        BlockAllocator alloc_{ };

        void operator()(Block* block) const
        {
            if (block == nullptr) return;
            ElementAllocator element_alloc_(alloc_);
            ElementAllocTraits::deallocate(element_alloc_, block->elements_, block->capacity_);
            BlockAllocTraits::deallocate(alloc_, block,1);
        }
    };

    /* --- Base Iterator Class --- */
public:
    using value_type      = T;
    using allocator_type  = Allocator;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = typename AllocTraits::pointer;
    using const_pointer   = typename AllocTraits::const_pointer;

    template<bool Const>
    class base_iterator
    {
    public:
        using value_type       = T;
        using iterator_concept = std::forward_iterator_tag;
        using difference_type  = std::ptrdiff_t;
        using pointer          = std::conditional_t<Const, const T*, T*>;
        using reference        = std::conditional_t<Const, const T&, T&>;

        base_iterator() = default;
        operator base_iterator<true>() const
        {
            return base_iterator<true>(current_block_, idx_in_block_);
        }

        reference operator*() const
        {
            return current_block_->elements_[idx_in_block_].data;
        }

        pointer operator->() const
        {
            return current_block_->elements_[idx_in_block_].data;
        }

        // TODO: forwarding
        base_iterator& operator++();
        base_iterator  operator++(int) const
        {
            base_iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const base_iterator& other) const
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
    BlockPtr first_block_{ nullptr };
    Block*   last_block_{ nullptr };

    Element*  free_list_head_{ nullptr };
    Allocator allocator_{ };

    size_t size_{ };
    size_t capacity_{ };
    size_t next_block_capacity_{ 8 };


    /* --- Hive Special Member Functions --- */
public:
    explicit hive(const Allocator& alloc = Allocator())
        : allocator_(alloc)
    { }

    ~hive() { clear(); }

    void clear() noexcept
    {
        if (first_block_ == nullptr) return;

        Block* curr_block = first_block_;
        while (curr_block != nullptr)
        {
            for (int i{}; i<curr_block->highest_untouched_; ++i)
            {
                if (curr_block->elements_[i].state_ == Element::State::Active)
                    AllocTraits::destroy(allocator_, curr_block->elements_[i].data);

            }

            curr_block = curr_block->next;
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
    [[nodiscard]] size_t size() const { return size_; }
    [[nodiscard]] size_t capacity() const { return capacity_; }

    [[nodiscard]] iterator begin() noexcept;
    [[nodiscard]] const_iterator begin() const noexcept;

    [[nodiscard]] iterator end() noexcept { return iterator(nullptr, 0); }
    [[nodiscard]] const_iterator end() const noexcept { return const_iterator(nullptr, 0); }
    
    template<typename... Args>
    iterator emplace(Args&&... args);

    iterator erase(const_iterator it);

private:
    void add_block();
    // what if prev_active called on idx 0 of first block?
    // iterator containing nullptr? exception?
    iterator prev_active(Block* block, size_t idx);

};

    /* --- Thick Functions --- */

template<typename T, typename Allocator>
void hive<T, Allocator>::add_block()
{
    BlockAllocator block_alloc{ allocator_ };
    ElementAllocator elem_alloc{ allocator_ };

    BlockPtr new_block{ BlockAllocTraits::allocate(block_alloc, 1), BlockDeleter{ block_alloc }};
    new_block->elements_ = ElementAllocTraits::allocate(elem_alloc, next_block_capacity_);

    capacity_ += next_block_capacity_;
    next_block_capacity_ *= 2;

    if (last_block_ == nullptr) [[unlikely]]
    {
        first_block_ = std::move(new_block);
        last_block_ = first_block_.get();
    }
    else
    {
        last_block_->next = std::move(new_block);
        last_block_ = last_block_->next.get();
    }
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
            if (curr_block->elements_[idx].state_ == Element::State::Active)
                return iterator(curr_block, idx);
        }
        curr_block = curr_block->next;
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
            if (curr_block->elements_[idx].state_ == Element::State::Active)
                return const_iterator(curr_block, idx);
        }
        curr_block = curr_block->next;
    }

    return end();
}