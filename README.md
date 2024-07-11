# News Broadcasting System

## Overview
This project simulates a news broadcasting system using concurrent programming and synchronization mechanisms. The system consists of producers generating news items, a dispatcher sorting these items, co-editors processing them, and a screen manager displaying the news items.

## Structure
The project includes the following files:

- main.c: The main program implementing the simulation.
- bounded_queue.h, bounded_queue.c: Implementation of a bounded queue for producers.
- unbounded_queue.h, unbounded_queue.c: Implementation of an unbounded queue for consumers (co-editors).
- newsitem.h: Definition of the newsItem struct.
- Makefile: To compile the program.
- config.txt: Configuration file specifying the number of producers, their production limits, and queue sizes.

## Compilation
To compile the project, use the provided Makefile. Run the following command in the project directory ``` make ```.
This will generate an executable file named ex3.out.

The config.txt file should contain the configuration for producers and the co-editors queue size. 
Then, run the program with the configuration file: ``` ./ex3.out config.txt ```

![image](https://github.com/ilanitb16/Concurrent_Programming/assets/97344492/b3bd0ad7-8b2f-4d5e-a180-7660383e9800)

## Example
An example configuration file is:
```
1 30 5
2 25 3
3 16 30
17
```
### This configuration specifies:
Producer 1 produces 30 items with a queue size of 5.
Producer 2 produces 25 items with a queue size of 3.
Producer 3 produces 16 items with a queue size of 30.
Co-Editor queue size is 17.

## Output
The output will consist of lines indicating the producer ID, the type of news, and the sequence number of that type. The final line will be "DONE" when all news items are processed and displayed.
