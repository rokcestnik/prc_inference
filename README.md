Infer the [phase response curve](http://www.scholarpedia.org/article/Phase_response_curve) from observations of the signal
==========================================================================================================================

For this you need an observation of a scalar signal and its scalar perturbation in time. 
Note that if you have the system equations there is a different, straightforward algorithm that you should use, see method [oscillator_PRC()](https://github.com/rokcestnik/oscillator_snap/blob/master/oscillator_snap/oscillator_gauge.py). 

The algorithm is described in the paper [Inferring the phase response curve from observation of a continuously perturbed oscillator](https://www.nature.com/articles/s41598-018-32069-y).
It is implemented in c and uses [Eigen](http://eigen.tuxfamily.org/) for a short linear algebra part.

### Setup

To set the code up just download (or clone) the repository, add [Eigen](http://eigen.tuxfamily.org/) to the folder 'Eigen' and you should be all set. 

### Execute

To run it simply put the data in the 'data' folder and (on Linux at least) run the bash script [run.bash](run.bash):

```
bash run.bash
```
