

# Data Generator

Your task is to implement a simple synthetic dataset generator. This should be accessible via a well defined API. Your first task is to read something about data generation (for databases) in general. Make yourself familiar with the topic.
For this lecture we will use a simple random or normal distribution as generation technique. This means the focus does not lay on the generation method, but on the code around.
Key requirements:
* reasonable encapsulation of code
* good documentation
* readable code (maybe you need to read some best practices and try some things)
* easy usability (at least command-line)
* easy exchangeable entities (as easy as possible to exchange for example the generation method)
* flexibility for output (and maybe input)
* utilization of the learned skills (there will be hints when a special skill should be visible in the code)
* usability as library
* at least being prepared for writing generated data to a database (maybe even the actual doing...)

This task will be done by each student on its own. Every student has to hand own code at the end of the semester. You decide when and how you want to progress with the project. Of course we advice to incrementally work on it with every new lecture topic. There won’t be any comparisons or solutions from our side. This task is for you. But of course you are allowed to ask questions.
The advice for this week is to create an initial class structure for the generator and an API prototype. Do not fear mistakes. They will help you learn.


# Questions

* Output format? As SQL? csv
* ober- und untergrenze, anzahl columns, rows, deterministic by seed or random

# TODOs

* [x] parameter parser
* [x] measure difference between shell pipe and direct file writing ---> performance difference of 2-4%
* [x] normal distribution
* [x] sql output
* [x] comments
* [x] reproducible output (seed)
* [x] comment with used command line parameters
* [x] namespace for lib
* [x] turn on compiler warnings/errors
* [x] data --> single dimensional vector
* [ ] ~~tests?~~
* [x] change executable name
* [x] rename model.hpp
* [x] shouldn't crash with n,c == 0,0 (both in lib and in cli)
* [ ] use iterators for output
* [ ] ~~output bool in the right way~~
* [x] tablename option
* [x] readme
* [ ] ~~use inheritance instead of switches in main.cpp?~~