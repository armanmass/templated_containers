#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>
#include <cstddef>

template <typename T, typename Allocator = std::allocator<T>>
class Vector
{   
    using value_type             = T;
    using allocator_type         = Allocator;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer          = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator               = typename std::allocator_traits<Allocator>::pointer;
    using const_iterator         = typename std::allocator_traits<Allocator>::const_pointer;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    Vector() { }
    ~Vector() { delete[] data_; }

    Vector(const Vector& other)
    {

    }

    Vector& operator=(const Vector& other)
    {
        if (this == &other)
            return *this;

        size_ = other.size_;
        capacity_ = other.capacity_;


        //TODO: deallocate/destruct old data
        //allocate for new and copy
        delete[] data_;
        data_ = new value_type[capacity_];

        for (int i{}; i<size_; ++i)
            data_[i] = other.data_[i];

        return *this;
    }

    [[nodiscard]] constexpr reference operator[](size_type idx) { return data_[idx]; }

    [[nodiscard]] constexpr reference at(size_type idx)
    {
        if (idx < 0 || idx >= size_)
            throw std::out_of_range("Index out of range.");
        return data_[idx];
    }

    [[nodiscard]] constexpr reference front() { return data_[0]; }
    [[nodiscard]] constexpr reference back() { return data_[size_-1]; }
    [[nodiscard]] constexpr pointer data() { return data_; }



    template <typename U>
    constexpr void push_back(U&& val) noexcept
    {
        grow_if_full();
        data_[size_] = std::forward<U>(val);
        ++size_;
    }

    constexpr void pop_back() noexcept
    {
        --size_;
        //TODO: destruct/dealloc old object
    }

    constexpr void swap(Vector& other)
    {
        using std::swap;
        swap(alloc_, other.alloc_);
        swap(data_, other.data_);
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
    }

    template<typename U>
    constexpr void insert(size_type idx, U&& val)
    {
        grow_if_full();
        for (size_type i{size_}; i > idx; --i)
            data_[i] = std::move(data_[i-1]); 

        data_[idx] = std::forward<U>(val);
        ++size_;
    }


    [[nodiscard]] constexpr size_type size() const noexcept { return size_; } 
    [[nodiscard]] constexpr size_type capacity() const noexcept { return capacity_; } 
    [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; } 

    constexpr void clear() noexcept
    {
        //TODO: deallocate
        delete[] data_;
        size_ = 0;
        capacity_ = 8;
    }

    constexpr void reserve(size_type new_capacity_)
    {
        if (new_capacity_ > capacity_)
        {
            resize(new_capacity_);
        }
    }

    void shrink_to_fit() { resize(size_); }

    void resize(size_type new_capacity_)
    {
        capacity_ = new_capacity_;
        size_ = std::min(size_, capacity_);
        value_type* new_data_ = new value_type[capacity_];

        for (int i{}; i<size_; ++i)
        {
            new_data_[i] = std::move(data_[i]);
        }

        delete data_;
        data_ = new_data_;
    }

private: 
    [[no_unique_address]] allocator_type alloc_;
    value_type* data_{ nullptr };
    size_type size_{};
    size_type capacity_{};

    void grow_if_full() { if (size_ == capacity_) { resize(capacity_*2); }  }
};