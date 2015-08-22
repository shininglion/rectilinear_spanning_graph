#ifndef VALUE_TYPE_HPP
#define VALUE_TYPE_HPP


// value is true when T is a container with value_type defined in structure
// otherwise, T is a primitive type
template <class T>
struct has_value_type
{
    typedef char true_type;
    typedef char false_type[2];

    // template not available if there's no nested value_type in U's scope
    template <class U>
    static true_type test(typename U::value_type*);

    // fallback
    template <class U>
    static false_type& test(...);

    // tests which overload of test is chosen for T
    static const bool value = sizeof(test<T>(0)) == sizeof(true_type);
};

// pre-declaration for partial specialization
template <class T, bool b>
struct value_type_impl;

// a primitive type
template <class T>
struct value_type_impl<T, false> //if T doesn't define value_type
{
    typedef T type;
};

// a container
template <class T>
struct value_type_impl<T, true> //if T defines value_type
{
    typedef typename T::value_type type;
};


/** get value_type of a container (if T is a primitive type, just return T) **/
template <class T>
struct ValueType : value_type_impl<T, has_value_type<T>::value> {};

#endif
