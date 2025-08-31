# advancedTopicsInProgramming

A repository for the course Advanced Topics in Programming in TAU.

## Names & IDs

* Ariel Berant -
* Ido Zacharia -

## Formats

### Inputs

Input files notation:

* Player 1 tank: '1'
* Player 2 tank: '2'
* Mine: '@'
* Wall: '#'
* Empty space: ' '

### Outputs

Output files are generated as "output_" + input_file_name + ".txt" if -verbose flag is in input, as required. The directory in which they are generated is the same directory as the input file's directory(if the given input file can be accessed, that is).

Errors in the input file are all put into one file named "input_errors.txt" as required.

At the end of a succesful run, nothing will be printed to the terminal, and the game results, logs, and summary(see in bonus.txt) will all be in the output file itself.

If any errors occured, they will either:

* Be printed to input_errors, if they are recoverable.
* Be printed to terminal(optionally input_file), if unrecoverable.

## Compiling the project

To compile the code, simply run ```make``` in the terminal whilst in the desired directory. This will create the .so file\simulator executable, which you can then give as input to a simulator executable\run on various inputs.

To clean executable\.so file generated in the local directory, run ```make clean```.

## General Notes

### File handling

The program tries to delete pre-existing output\input errors files by default. To prevent this behaviour, comment the lines in gameManager.cpp::createMap.

Note that if undeleted, the errors will be appended to the pre-existing files.

### Bugs not fixed in time

The GameManager for our project(for some unknown reason) creates a segfault and returns an incomplete map(missing the tanks, maybe all moving objects, idk).
