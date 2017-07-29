#include <iostream>
#include <utility>

/*
//http://isocpp.org/blog/2012/11/universal-references-in-c11-scott-meyers
int y(int &) { return 1; }
int y(int &&) { return 2; }

template <class T> int f(T &&x) { return y(x); }
template <class T> int g(T &&x) { return y(std::move(x)); }
template <class T> int h(T &&x) { return y(std::forward<T>(x)); }

int main() {
  int i = 10;
  std::cout << f(i) << f(20);
  std::cout << g(i) << g(20);
  std::cout << h(i) << h(20);
  return 0;
}*/

/*
#include <string>
#include <iostream>
#include <utility>

struct A
{
  std::string s;
  A() : s("test") { }
  A(const A& o) : s(o.s) { std::cout << "move failed!\n"; }
  A(A&& o) : s(std::move(o.s)) { }
  A& operator=(const A& other)
  {
    s = other.s;
    std::cout << "copy assigned\n";
    return *this;
  }
  A& operator=(A&& other)
  {
    s = std::move(other.s);
    std::cout << "move assigned\n";
    return *this;
  }
};

A f(A a) { return a; }

struct B : A
{
  std::string s2;
  int n;
  // implicit move assignment operator B& B::operator=(B&&)
  // calls A's move assignment operator
  // calls s2's move assignment operator
  // and makes a bitwise copy of n
};

struct C : B
{
  ~C() { } // destructor prevents implicit move assignment
};

struct D : B
{
  D() { }
  ~D() { } // destructor would prevent implicit move assignment
  D& operator=(D&&) = default; // force a move assignment anyway 
};

int main()
{
  A a1, a2;
  std::cout << "Trying to move-assign A from rvalue temporary\n";
  a1 = f(A()); // move-assignment from rvalue temporary
  std::cout << "Trying to move-assign A from xvalue\n";
  a2 = std::move(a1); // move-assignment from xvalue

  std::cout << "Trying to move-assign B\n";
  B b1, b2;
  std::cout << "Before move, b1.s = \"" << b1.s << "\"\n";
  b2 = std::move(b1); // calls implicit move assignment
  std::cout << "After move, b1.s = \"" << b1.s << "\"\n";

  std::cout << "Trying to move-assign C\n";
  C c1, c2;
  c2 = std::move(c1); // calls the copy assignment operator

  std::cout << "Trying to move-assign D\n";
  D d1, d2;
  d2 = std::move(d1);
}*/

/*
#include <iostream>
using namespace std;

template<typename T>
void adl(T)
{
  cout << "T";
}

struct S
{
};

template<typename T>
void call_adl(T t)
{
  adl(S());
  adl(t);
}

void adl(S)
{
  cout << "S";
}

int main()
{
  call_adl(S());
}*/


//§14.6¶9 states: "When looking for the declaration of a name used in a 
//template definition, the usual lookup rules (§3.4.1, §3.4.2) are used for non-dependent names.
//The lookup of names dependent on the template parameters is postponed until the actual template
//argument is known (§14.6.2)." The first call to adl is a non-dependent call,
//so it is looked up at the time of definition of the function template.
//The resolution of the second call is deferred until the template is instantiated because it depends on a template parameter.

//template<typename T> void call_adl_function(T t)
//{
//adl(S()); // Independent, looks up adl now.
//adl(t); // Dependent, looks up adl later.
//}

//When adl is being looked up at the time of definition of the function template,
//the only version of adl that exists is the templated adl(T).
//Specifically, adl(S) does not exist yet, and is not a candidate.
//Note: At the time of writing, this program does not confirm to the standard in some recent versions of Visual Studio's C++ compiler.

/*
#include <iostream>
#include <type_traits>

int main() {
  if (std::is_signed<char>::value) {
    std::cout << std::is_same<char, signed char>::value;
  }
  else {
    std::cout << std::is_same<char, unsigned char>::value;
  }
}*/

/*
#include <iostream>
using namespace std;

class A
{
public:
  A() { cout << "A"; }
  A(const A &) { cout << "a"; }
};

class B : public virtual A
{
public:
  B() { cout << "B"; }
  B(const B &) { cout << "b"; }
};

class C : public virtual A
{
public:
  C() { cout << "C"; }
  C(const C &) { cout << "c"; }
};

class D : public B, C
{
public:
  D() { cout << "D"; }
  D(const D &) { cout << "d"; }
};

int main()
{
  D d1;
  D d2(d1);
}*/

/*
#include <iostream>
#include <memory>
#include <vector>

class C {
public:
  void foo() { std::cout << "A"; }
  void foo() const { std::cout << "B"; }
};

struct S {
  std::vector<C> v;
  std::unique_ptr<C> u;
  C *const p;

  S()
    : v(1)
    , u(new C())
    , p(u.get())
  {}
};

int main() {
  S s;
  const S &r = s;

  s.v[0].foo();
  s.u->foo();
  s.p->foo();

  r.v[0].foo();
  r.u->foo();
  r.p->foo();
}*/

/*
#include <iostream>
#include <utility>

struct A
{
  A() { std::cout << "1"; }
  A(const A&) { std::cout << "2"; }
  A(A&&) { std::cout << "3"; }
};

struct B
{
  A a;
  B() { std::cout << "4"; }
  B(const B& b) : a(b.a) { std::cout << "5"; }
  B(B&& b) :
    a(std::move(b.a)) //if there is used std::move then 1436
  { std::cout << "6"; }
};

int main()
{
  B b1;
  B b2 = std::move(b1);
}*/

//1. aliases are considered as the new type
//2. preferable to call the ctor if the time is direct-initialiation. Direct initialization {} too.

/*
#include <iostream>
#include <type_traits>

using namespace std;

int main()
{
  int i, &j = i;
  [=]
  {
    cout << is_same<decltype    ((j)), int         >::value
      << is_same<decltype   (((j))), int      &  >::value
      << is_same<decltype  ((((j)))), int const&  >::value
      << is_same<decltype (((((j))))), int      && >::value
      << is_same<decltype((((((j)))))), int const&& >::value;
  }();
}
00100

§5.1.2¶18 says
Every occurrence of decltype((x)) where x is a possibly parenthesized id-expression
that names an entity of automatic storage duration is treated as
if x were transformed into an access to a corresponding data member of
the closure type that would have been declared if x were an odr-use of the denoted entity.
So additional parentheses, as the in the code snippet above, are ignored.
The member of the closure type corresponding to the as-if-captured j will be not a reference,
but will have the referenced type of the reference, since it is captured by copy (§5.1.2¶14).

Since the lambda is not declared mutable, the overloaded operator() of the closure type will be a const member function.
5.1.2¶5: "The closure type for a lambda-expression has a public inline function call operator (...)
This function call operator is declared const if and only if the lambda-expression’s parameter-declaration-clause is not followed by mutable."

Since the expression for decltype is a parenthesized lvalue expression,
§7.1.6.2¶4 has this to say: "The type denoted by decltype(e) is (...) T&,
where T is the type of e;" As the expression occurs inside a const member function,
the expression is const, and decltype((j)) denotes int const&. See also the example in §5.1.2¶18.
*/

// 4.
/*
#include <iostream>
struct X {
X() { std::cout << "X"; }
};

int main()
{ 
  X x(); // X x(); is a function prototype, not a variable definition.
}*/

/*
#include <initializer_list>
#include <iostream>

struct A {
  A() { std::cout << "1"; }

  A(int) { std::cout << "2"; }

  A(std::initializer_list<int>) { std::cout << "3"; }
};

int main(int argc, char *argv[]) {
  A a1;
  A a2{};  //If the initializer list has no elements and T is a class type with a default constructor
  A a3{ 1 };
  A a4{ 1, 2 };
}
*/

//5. volatile a main() { std::cout << a + a; } Undefined behaviour for non-sequence access

//6.
/*
#include <iostream>

struct X {
  X() { std::cout << "X"; }
};

struct Y {
  Y(const X &x) { std::cout << "Y"; }
  void f() { std::cout << "f"; }
};

int main() {
  Y y(X());
  // This could be interpreted as a a variable definition (which was the intention of the programmer in this example), or as a definition of a function y,
  // returning an object of type Y, taking a function (with no arguments, returning an object of type X) as its argument.
  //  The compiler is required by the standard to choose the second interpretation, which means that y.f() does not compile(since y is now a function, not an object of type Y).
  y.f();
}*/

/*
#include <iostream>

int f(int &a, int &b) {
  a = 3;
  b = 4;
  return a + b;
}

int main() {
  int a = 1;
  int b = 2;
  int c = f(a, a);
  std::cout << a << b << c;
}

//When f() is called with a as both parameters,
//both arguments refer to the same variable.
//This is known as aliasing. First, a is set to 3,
//then a is set to 4, then 4+4 is returned. b is never modified.
*/

/*
#include <iostream>

void print(char const *str) { std::cout << str; }
void print(short num) { std::cout << num; }

int main() {
print("abc");
print(0);
print('A');
}

"If there is exactly one viable function that is a better function than all other viable functions, then it is the one selected by overload resolution; otherwise the call is ill-formed".

*/

/*
#include <iostream>
template <class T> void f(T &i) { std::cout << 1; }
template <> void f(const int &i) { std::cout << 2; }
int main() {
  int i = 42;
  f(i);
}*/

/*
#include <iostream>
class C {
public:
  C(int i) : i(i) { std::cout << i; }
  ~C() { std::cout << i + 5; }

private:
  int i;
};

int main() {
  const C &c = C(1);
  C(2);
}*/

/*
#include <iostream>

int main() {
int a = 0;
decltype((a)) b = a;
b++;
std::cout << a << b;
}

//— otherwise, if e is an xvalue, decltype(e) is T&&, where T is the type of e;
//— otherwise, if e is an lvalue, decltype(e) is T&, where T is the type of e;
//— otherwise, decltype(e) is the type of e."
// Because a is encapsulated in parentheses, it doesn't qualify for the first case, it is treated as an lvalue, therefore b's type is int&, not int.
*/

/*
#include <iostream>

int main() {
  int a = 0;
  decltype(a) b = a;
  b++;
  std::cout << a << b;
}

the type denoted by decltype(e) is defined as follows: if e is an unparenthesized id-expression decletype(e) is the type of the entity named by e
*/


//elison copy. return value optimization

/*
#include <iostream>
struct E
{
  E() { std::cout << "1"; }
  E(const E&) { std::cout << "2"; }
  ~E() { std::cout << "3"; }
};

E f()
{
  return E();
}

int main()
{
  f();
}

In f(), an E object is constructed, and 1 is printed.
This object is then returned to main(), and one could expect the copy constructor to be called, printing 2.
However, §6.6.3¶2 in the standard says "Note: A copy or move operation associated with a return statement may be elided",
meaning that the implementation is free to skip the copy constructor. This is optional, and unspecified behaviour.
Most implementations will elide the copy constructor and output 13,
but may be urged not to so with the -fno-elide-constructors option and output 1233.
For more information about the return value optimisation,
and an investigation of which implementations use it, see http://blog.knatten.org/2011/08/26/dont-be-afraid-of-returning-by-value-know-the-return-value-optimization/


*/

/*
#include <iostream>
#include <limits>

int main()
{
  int N[] = { 0,0,0 };

  if (std::numeric_limits<long int>::digits == 63 and
    std::numeric_limits<int>::digits == 31 and
    std::numeric_limits<unsigned int>::digits == 32)
  {
    for (long int i = -0xffffffff; i; --i)
    {
      N[i] = 1;
    }
  }
  else
  {
    N[1] = 1;
  }

  std::cout << N[0] << N[1] << N[2];
}

As the else part of the branch is obvious, we concentrate on the if part and make the assumptions present in the condition.
§2.14.2 in the standard: "The type of an integer literal is the first of the corresponding list in
Table 6." [Table 6: int, unsigned int, long int, unsigned long int … for hexadecimal literals --end Table] in which its value can be represented."
Since the literal 0xffffffff needs 32 digits,
it can be represented as an unsigned int but not as a signed int,
and is of type unsigned int. But what happens with the negative of an unsigned integer?
§5.3.1 in the standard: "The negative of an unsigned quantity is computed by subtracting its value from 2^n ,
where n is the number of bits in the promoted operand." Here n is 32, and we get:
2^32 - 0xffffffff = 4294967296 - 4294967295 = 1
So i is initialised to 1, and N[1] is the only element accessed in the loop.
(The second time around the loop, i is 0, which evaluates to false, and the loop terminates.)

*/


#include <iostream>
#include <string>
#include <future>

int main() {
  std::string x = "x";

  std::async(std::launch::async, [&x]() {
    x = "y";
  });
  std::async(std::launch::async, [&x]() {
    x = "z";
  });

  std::cout << x;
}


//The destructor of a future returned from async is required to
//block until the async task has finished(see elaboration below).
//Since we don't assign the futures that are returned from async() to anything,
//they are destroyed at the end of the full expression (at the end of the line in this case).
//§12.2¶3 in the standard: "Temporary objects are destroyed as the last step in
//evaluating the full-expression (1.9) that (lexically) contains the point where they were created."
//This means that the first async call is guaranteed to finish execution before async() is called the second time, so, while the assignments themselves may happen in different threads, they are synchronized.

//Elaboration on synchronization :
//According to § 30.6.8¶5 of the standard :
//Synchronization:
//[...]
//If the implementation chooses the launch::async policy,
//— the associated thread completion synchronizes with(1.10) the return from the first function that successfully detects the ready status of the shared state or with the return from the last function that releases the shared state, whichever happens first.
//In this case, the destructor of std::future<> returned by the async() call is "the last function that releases the shared state", therefore it synchronizes with(waits for) the thread completion.
//Scott Meyers writes more about this http://scottmeyers.blogspot.com/2013/03/stdfutures-from-stdasync-arent-special.html
