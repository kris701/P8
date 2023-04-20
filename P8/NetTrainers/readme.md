# Net Trainers
This folder contains different implementations of trainers and testers for neural networks.

You can get specific trainer by using `NetTrainerBuilder.py`.

Each NetTrainer have two methods that they override from a base class, being:
`Train(self)` and `Test(self)`

The train method trains the model (if possible), and the test method tests the accuracy of the model on the test dataset

A net trainer takes in a set of `NetOptions`. This is a object that contains a lot of information on how the trainer should run (e.g. Epochs, Dataset name, etc.).

## Net trainer list
There are currently two different net trainers:
* KNN
* ProtoNet

The KNN is more or less just here as a sanity check for the protonet.
The KNN is written in c++, while the protonet is in python using Torch