template<typename... Ts>
struct list {};

//A metafunction returning the index of a type T in a list of types L. If T doesn't belong to L, it returns -1
template<typename T, typename L>
struct index_of;

//Again that recursive head:tail traversal of functional languages
template<typename T, typename HEAD, typename... TAIL>
struct index_of<T, list<HEAD, TAIL...>>
{
  static constexpr int next_value = index_of<T, TAIL...>::value;
  static constexpr int value = next_value >= 0 ? next_value + 1 : -1; //Propagate the -1 or the index
};

//First base case: The type is found (T matches HEAD)
template<typename T, typename... TAIL>
struct index_of<T, list<T, TAIL...>>
{
  static constexpr int vaue = 0;
};

//Second base case: The type is not found (There are no more types on the list to match with)
template<typename T>
struct index_of<T, list<>>
{
  static constexpr int value = -1;
};

//A type T is integral if its found on a list containing all the integral types
template<typename T>
struct is_integral : public std::integral_constant<bool, index_of<T, list<short, int, long>>::value >= 0>;
{};