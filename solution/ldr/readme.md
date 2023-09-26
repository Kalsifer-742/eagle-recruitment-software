# Documentation

I could not find a datasheet for this component.
I think that's because it's not meant to be accurate.
One option was to go for a qualitative approach reporting macro light levels: dim, bright, ecc...
The other option wich I choose was to calibrate the sensor my self.

To do this i needed a Lux metere wich I don't have.
Luckily I found a cool [app](https://play.google.com/store/apps/details?id=com.doggoapps.luxlight) which could use my phone light sensor to measure luxs.

*This [tutorial](https://www.allaboutcircuits.com/projects/design-a-luxmeter-using-a-light-dependent-resistor/) was also of great interest giving me an example of what I could do to achieve my goal.*

So I took some measuraments and then elaborated the results in some graphs.
Then I tried doing some curve fitting which leaded me to mixed results.

In the end I found a function that characterizes the sensor well enough.
It's not perfect by any means but it's better then nothing.

## Data and Code

- [Here](https://docs.google.com/spreadsheets/d/e/2PACX-1vTnh1X9CsZkK5S5eDIxaffOZ9EeqaGZKAWHTWqnwucb3Mn0FRA0lajspqk6cpvHASq8e8VKB7fXO3c9/pubhtml) you can find the results.
- [small program](ldr_reader.ino) to read the sensor
- [small python script](./exp_curv_fitting.py) to do curve fitting
- [curve fitting website](https://mycurvefit.com/index.html)