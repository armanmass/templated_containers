#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <ranges>
#include <concepts>


template <typename T, typename Allocator = std::allocator<T>>
class Vector
{   
public:
    template <bool IsConst>
    class Iterator;

    using value_type             = T;
    using allocator_type         = Allocator;
    using alloc_traits           = std::allocator_traits<Allocator>;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer          = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator               = Iterator<false>;
    using const_iterator         = Iterator<true>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
/***********************************
      Special Member Functions 
***********************************/
    constexpr Vector() { }
    ~Vector() 
    { 
        clear(); 
        alloc_traits::deallocate(alloc_, data_, capacity_);
    }

    explicit Vector(const allocator_type& alloc)
        : alloc_(alloc)
    { }

    explicit Vector(size_type n)
        : size_(n),
          capacity_(n)
    {
        data_ = alloc_traits::allocate(alloc_, capacity_);
        std::uninitialized_value_construct_n(data_, size_);
    }

    explicit Vector(size_type n, const_reference val)
        : size_(n),
          capacity_(n)
    {
        data_ = alloc_traits::allocate(alloc_, capacity_);
        std::uninitialized_fill_n(data_, size_, val);
    }

    explicit Vector(std::initializer_list<value_type> init)
        : size_(init.size()),
          capacity_(init.size())
    {
        data_ = alloc_traits::allocate(alloc_, capacity_);
        std::ranges::uninitialized_copy(init, std::ranges::subrange(data_, data_+size_));
    }


    Vector(const Vector& other)
        : size_(other.size_),
          capacity_(other.size_)
    {
        data_ = alloc_traits::allocate(alloc_, capacity_);
        std::uninitialized_copy_n(other.data_, size_, data_);
    }


    Vector& operator=(const Vector& other)
    {
        if (this != &other)
        {
            Vector temp{other};
            swap(temp);
        }

        return *this;
    }


    constexpr Vector(Vector&& other) noexcept
        : alloc_(other.alloc_),
          data_(other.data_),
          size_(other.size_),
          capacity_(other.capacity_)
    {  other.moved_from_state_(); }


    constexpr Vector& operator=(Vector&& other) noexcept
    {
        if (this == &other)
            return *this;

        clear();
        alloc_traits::deallocate(alloc_, data_, capacity_);

        swap(other);
        other.moved_from_state_();

        return *this;
    }


    void assign(size_type count, const value_type& val)
    {
        clear();
        if (count > capacity_)
        {
            alloc_traits::deallocate(alloc_, data_, capacity_);
            data_ = alloc_traits::allocate(alloc_, count);
            capacity_ = count;
        }
        std::uninitialized_fill_n(data_, count, val);
        size_ = count;
    }


    [[nodiscard]] allocator_type get_allocator() const noexcept { return alloc_; }

/***********************************
          Element Access 
***********************************/
    [[nodiscard]] constexpr reference at(size_type idx)
    {
        if (idx >= size_) throw std::out_of_range("Index out of range.");
        return data_[idx];
    }

    [[nodiscard]] constexpr const_reference at(size_type idx) const
    {
        if (idx >= size_) throw std::out_of_range("Index out of range.");
        return data_[idx];
    }

    [[nodiscard]] constexpr reference operator[](size_type idx) noexcept { return data_[idx]; }
    [[nodiscard]] constexpr const_reference operator[](size_type idx) const noexcept { return data_[idx]; }

    [[nodiscard]] constexpr reference front() noexcept { return data_[0]; }
    [[nodiscard]] constexpr const_reference front() const noexcept { return data_[0]; }

    [[nodiscard]] constexpr reference back() noexcept { return data_[size_-1]; }
    [[nodiscard]] constexpr const_reference back() const noexcept { return data_[size_-1]; }

    [[nodiscard]] constexpr pointer data() noexcept { return data_; }
    [[nodiscard]] constexpr const_pointer data() const noexcept { return data_; }

/***********************************
             Iterators
***********************************/
    [[nodiscard]] constexpr iterator begin() noexcept { return iterator{data_}; }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return const_iterator{data_}; }

    [[nodiscard]] constexpr iterator end() noexcept { return iterator{data_+size_}; }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return const_iterator{data_+size_}; }

    [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return reverse_iterator{end()}; }
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }
    
    [[nodiscard]] constexpr reverse_iterator rend() noexcept { return reverse_iterator{begin()}; }
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }
    
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }


/***********************************
             Capacity 
***********************************/
    [[nodiscard]] constexpr size_type size() const noexcept { return size_; } 
    [[nodiscard]] constexpr size_type capacity() const noexcept { return capacity_; } 
    [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; } 

    constexpr void reserve(size_type new_capacity_)
    {
        if (new_capacity_ <= capacity_) 
            return;

        pointer new_data_ = alloc_traits::allocate(alloc_, new_capacity_);
        std::uninitialized_move_n(data_, size_, new_data_);

        std::destroy(data_, data_+size_);
        alloc_traits::deallocate(alloc_, data_, capacity_);

        data_ = new_data_;
        capacity_ = new_capacity_;
    }

    constexpr void shrink_to_fit() 
    { 
        if (size_ == capacity_) return;
        pointer new_data_ = alloc_traits::allocate(alloc_, size_);
        std::uninitialized_move_n(data_, size_, new_data_);
        
        std::destroy(data_, data_+size_); 
        alloc_traits::deallocate(alloc_, data_, capacity_);

        data_ = new_data_;
        capacity_ = size_;
    }


/***********************************
             Modifiers 
***********************************/

    constexpr void clear() noexcept
    {
        std::destroy(data_, data_+size_);
        size_ = 0;
    }


    template<typename U>
    constexpr iterator insert(const_iterator pos, U&& val)
    { return emplace(pos, std::forward<U>(val)); }


    template<typename... Args>
    constexpr iterator emplace(const_iterator pos, Args&&... args)
    {
        difference_type idx = pos - cbegin();

        grow_if_full_();

        std::move_backward(data_+idx, data_+size_, data_+size_+1);

        alloc_traits::construct(alloc_, data_+idx, std::forward<Args>(args)...);
        ++size_;

        return iterator{data_+idx};
    }


    template<typename... Args>
    constexpr iterator emplace_back(Args&&... args)
    { return emplace(cend(), std::forward<Args>(args)...); }


    constexpr iterator erase(const_iterator pos)
    {
        difference_type idx = pos-cbegin();

        std::move(data_+idx+1, data_+size_, data_+idx);

        std::destroy_at(data_+size_-1);
        --size_;

        return iterator{data_+idx};
    }


    template <typename U>
    constexpr void push_back(U&& val)
    { emplace_back(std::forward<U>(val)); }


    constexpr void pop_back() { std::destroy_at(data_ + --size_); }


    constexpr void resize(size_type new_size_)
    {
        
        if (new_size_ > capacity_)
        {
            reserve(new_size_);
            std::uninitialized_value_construct(data_+size_, data_+new_size_);
        }
        else
        {
            std::destroy(data_+new_size_, data_+size_);
        }

        size_ = new_size_;
    }


    constexpr void swap(Vector& other) noexcept
    {
        using std::swap;
        swap(alloc_, other.alloc_);
        swap(data_, other.data_);
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
    }


private: 
    static constexpr size_type INIT_CAPACITY{ 2 };

    [[no_unique_address]] allocator_type alloc_{};
    pointer   data_{ nullptr };
    size_type size_{};
    size_type capacity_{};

    constexpr void grow_if_full_() 
    { 
        if (size_ == capacity_) 
            reserve(capacity_ > 0 ? capacity_*2 : INIT_CAPACITY);  
    }


    constexpr void moved_from_state_() noexcept
    {
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }
};


template <typename T, typename Allocator>
template<bool IsConst>
class Vector<T, Allocator>::Iterator
{
public:
    using iterator_concept  = std::random_access_iterator_tag;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = Vector::difference_type;
    using value_type        = Vector::value_type;
    using pointer           = std::conditional_t<IsConst, Vector::const_pointer, Vector::pointer>;
    using reference         = std::conditional_t<IsConst, Vector::const_reference, Vector::reference>;

public:
    Iterator() = default;
    Iterator(pointer ptr)
        : ptr_(ptr)
    { }

    template <bool OtherConst>
        requires(IsConst && !OtherConst)
    Iterator(const Iterator<OtherConst>& other)
        : ptr_(other.ptr_)
    { }


    [[nodiscard]] constexpr reference operator*() const noexcept { return *ptr_; }
    [[nodiscard]] constexpr pointer operator->() const noexcept { return ptr_; }
    [[nodiscard]] constexpr reference operator[](size_type n) const noexcept { return *(ptr_ + n); }

    constexpr Iterator& operator++() noexcept 
    { 
        ++ptr_; 
        return *this;
    }

    constexpr Iterator operator++(int) noexcept 
    { 
        Iterator temp{ptr_};
        ptr_++;
        return temp;
    }

    constexpr Iterator& operator--() noexcept 
    { 
        --ptr_; 
        return *this;
    }

    constexpr Iterator operator--(int) noexcept 
    { 
        Iterator temp{ptr_};
        ptr_--;
        return temp;
    }

    constexpr Iterator& operator+=(difference_type n) noexcept 
    { 
        ptr_ += n;
        return *this;
    }

    constexpr Iterator& operator-=(difference_type n) noexcept
    {
        ptr_ -= n;
        return *this;
    }


    friend constexpr Iterator operator+(Iterator lhs, difference_type n) noexcept
    {
        lhs += n;
        return lhs;
    }

    friend constexpr Iterator operator+(difference_type n, Iterator lhs) noexcept
    { return lhs + n; }

    friend constexpr Iterator operator-(Iterator lhs, difference_type n)
    {
        lhs -= n;
        return lhs;
    }

    template <bool OtherConst>
    friend constexpr difference_type operator-(const Iterator& lhs, const Iterator<OtherConst>& rhs)
    { return lhs.ptr_ - rhs.ptr_; }

    template <bool OtherConst>
    constexpr bool operator==(const Iterator<OtherConst>& other) const noexcept
    { return ptr_ == other.ptr_; }

    template <bool OtherConst>
    constexpr auto operator<=>(const Iterator<OtherConst>& other) const noexcept
    { return ptr_ <=> other.ptr_; }

private:
    static_assert(std::indirectly_readable<Iterator>);
    pointer ptr_;
};


/***********************************
        Non-member functions 
***********************************/


template <typename T, typename Allocator>
constexpr void swap(Vector<T, Allocator>& lhs, Vector<T, Allocator>& rhs)
noexcept(noexcept(lhs.swap(rhs)))
{ lhs.swap(rhs); }

template <typename T, typename Allocator>
constexpr bool operator==(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs) noexcept
{ return std::ranges::equal(lhs, rhs); }

template <typename T, typename Allocator>
constexpr auto operator<=>(const Vector<T, Allocator>& lhs, const Vector<T, Allocator>& rhs) noexcept
{ return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()); }
