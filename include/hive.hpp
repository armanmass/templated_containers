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

    // allocator for type T objects inside elements inside blocks
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

    struct Block
    {
        size_t capacity_;
        Element* elements_;

        Block* next{ nullptr };
        Block* prev{ nullptr };

        size_t active_{ };
        size_t high_water_mark_{ };
    };

    struct BlockDeleter
    {
        BlockAllocator alloc_;

        void operator()(Block* b) const
        {
            if (b == nullptr) return;
            ElementAllocator element_alloc_(alloc_);
            ElementAllocTraits::deallocate(element_alloc_, b->elements_, b->capacity_);
            BlockAllocTraits::deallocate(alloc_, b,1);
        }
    };

public:
    using value_type      = T;
    using allocator_type  = Allocator;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = typename AllocTraits::pointer;
    using const_pointer   = typename AllocTraits::const_pointer;

    /* Base Iterator Class */

    template<bool Const>
    class base_iterator
    {
    public:
        using value_type       = T;
        using iterator_concept = std::forward_iterator_tag;
        using difference_type  = std::ptrdiff_t;
        using pointer          = std::conditional_t<Const, const T*, T*>;
        using reference        = std::conditional_t<Const, const T*, T*>;

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

private:
};