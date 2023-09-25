import numpy

# Your values
x = numpy.array([0.1, 17, 35, 55, 85, 105, 130, 140, 190, 250, 290, 370, 450, 500, 600, 800, 1000, 1100, 1200, 1500, 1800, 2000, 2300, 2600, 3000, 4500, 6000, 7500, 8000, 9000, 10000, 25000])
y = numpy.array([5, 5, 4.9, 4.25, 3.7, 3.45, 3.15, 3, 2.7, 2.5, 2.3, 2, 1.8, 1.6, 1.4, 1.3, 1.17, 1.12, 0.9, 0.87, 0.83, 0.7, 0.6, 0.5, 0.45, 0.43, 0.41, 0.4, 0.35, 0.25, 0.18, 0.16])

# Perform exponential fit
params = numpy.polyfit(numpy.log(x), y, 1)

# The 'params' variable will contain the parameters of the exponential function
# params[0] will be 'a' and params[1] will be 'b' in the equation y = exp(ax + b)

a = params[0]
b = params[1]

print(f"The exponential equation is: y = exp({a}x + {b})")

