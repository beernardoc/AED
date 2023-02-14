//
// Tomás Oliveira e Silva, AED, October 2021
//
// example of function overloading (same function name, different argument numbers and/or data types)
//

#include <iostream>

using std::cout; // make this symbol from the std namespace directly visible

void show(const int i)
{
  cout << "int: "
       << i
       << std::endl;
}

void show(const double d)
{
  cout << "double: "
       << d
       << std::endl;
}

void show(const char *s,const char *h = "string: ") // second argument with default value
{
  cout << h
       << s
       << std::endl;
}

void show(const char a){
  cout << "char: "
       << a
       << std::endl;
}


void show(const int *i, const int size = 3){
  cout << "Array: \n";
  for(int x = 0; x< size;x++){
    cout << "Array[" << x << "] = " << i[x] << std::endl;

  }


}


int main(void)
{
  int array[4] = {1,2,3,4};
  show(1.0);
  show("hello");
  show(-3);
  show("John","name: ");
  show('b');
  show(array,4);
  return 0;
}
