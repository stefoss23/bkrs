/*
The approxfunction is used to:
- avoid repetition of complicated calculations
- enable a function as product of a function (ex fourier transform).

Intial values are mapped as:

  g(x_i) = f(x_i)

between values x_i and x_(i+1), the function g(x) is linear. 

In order to find i, the values x_(i+1) - x_i are constant. 

Use:
To establish an approxfunction of the function f(x):
1) Create a std::vector<T> entry of the values of x that are to be calulcated. 
2) ApproxFunction<T> function = ApproxFunction(entry, value);
3) T output = function.output(x);

T can only be continuous values such as double, std::complex<double> or math_vector.
*/

#ifndef MATHEMATICS_APPROX_FUNCTION2_HPP
#define MATHEMATICS_APPROX_FUNCTION2_HPP

#include <vector>
#include <memory>
#include <complex>

#include <radsim/utils/utils.hpp>

#include <radsim/mathematics/math_vector.hpp>

namespace radsim {

template <class T>
class ApproxFunction {

  private:
    int num_values;
    std::vector<double> entry;
    std::vector<T> value;
    double diff;
    double first_entry;
    double last_entry;
    T initial_value; //For output, any input below FirstEntry returns this value
    T end_value; //For output, any input above LastEntry returns this value

    bool assertOrderedEntry()
    {
      bool ordered = true;
      if (entry[1] <= entry.front())
        ordered = false;
      first_entry = entry.front();
      last_entry  = entry.back();
      diff = (last_entry - first_entry + 0.0) / (entry.size() - 1.0);
      for (int n = 1; n < entry.size(); n++) {
        double cmp = entry[n] - entry[n-1];
        if (!double_equal(cmp, diff, 1e-5))
          ordered = false;
      }
      return ordered;
    }

   void assertType()
   {
      if (!(std::is_same<T, double>::value or std::is_same<T, std::complex<double> >::value or std::is_same<T, math_vector>::value))
        throw std::invalid_argument(__PRETTY_FUNCTION__ + std::string(": Appproxfunctons can be either double, std::complex<double> or math_vector."));
   }

    void initiate()
    {
      assertType();
      if (entry.size() < 2)
        throw std::invalid_argument(__PRETTY_FUNCTION__ + std::string(": Must have at least two entries in entry_arg."));
      if (entry.size() != value.size())
        throw std::invalid_argument(__PRETTY_FUNCTION__ + std::string(": vectors entry and value must be of equal size."));
      if (!assertOrderedEntry())
        throw std::invalid_argument(__PRETTY_FUNCTION__ + std::string(": entry_arg not equally spaced."));
    }

  public:
    ApproxFunction(T value_) :
      first_entry( 0 ),
      last_entry( 0 ),
      initial_value( value_ ),
      end_value( value_ ),
      entry( {0} ),
      value( {value_} )
    {
      assertType();
    }

    ApproxFunction(std::vector<double> entry_, std::vector<T> value_) :
      entry( move(entry_) ),
      value( move(value_) )
    {
      num_values = this->entry.size();
      initiate();
      this->initial_value = this->value.front();
      this->end_value = this->value.back();
    }

    ApproxFunction(std::vector<double> entry_, std::vector<T> value_, T initial_value, T end_value) :
      ApproxFunction(move(entry_), move(value_))
    {
      this->initial_value = initial_value;
      this->end_value = end_value;
    }

    ApproxFunction(ApproxFunction&& other) :
       num_values( other.num_values ),
       entry( move(other.entry) ),
       value( move(other.value) ),
       diff( other.diff ),
       first_entry( other.first_entry ),
       last_entry( other.last_entry ),
       initial_value( other.initial_value ),
       end_value( other.end_value )
    {
    }

    ApproxFunction(const ApproxFunction& other) = default;
    ApproxFunction& operator=(const ApproxFunction& other) = default;

    T output(double x) const {
      if (entry.size()) {
        if ( x < first_entry )
          return initial_value;
        if ( x >= last_entry )
          return end_value;
        int n = int((x - first_entry) / diff ) + 1;
        double w1 = (entry[n] - x) / diff;
        double w2 = (x - entry[n-1]) / diff;               
        return w1 * value[n-1] + w2 * value[n];
      }
      else
        throw std::logic_error(__PRETTY_FUNCTION__ + std::string(": cannot use an ApproxFunction that has been moved."));
    }


    std::vector<T> outputVector(std::vector<double> input) const 
    {
      int size = input.size();
      std::vector<T> output(size);
         
      for (int i = 0; i < size; i++)
        output[i] = this->output(input[i]);

      return output;
    }

    const std::vector<double>& getEntryVector() const {
      return entry;
    }
    const std::vector<T>& getValueVector() const {
      return value;
    }

};

typedef ApproxFunction<double>               DoubleApproxFunction;
typedef ApproxFunction<std::complex<double>> ComplexApproxFunction;
typedef ApproxFunction<math_vector>          VectorApproxFunction;

} //end nm bkradsim

#endif
