# To build the doxygen documentation
From the root directory run
   `$doxywizard doxyconf`
In the GUI Run Doxygen (the steps for this are OS/version dependant)
After it has completeded:
   `$cd doxygen/latex`
   `$make`
This will create a file called refman.pdf

# Good things to know

Running `size $(find $directory -type f -name "*.o")` inside of a compilation target folder gives you a list of the sizes of all the .o files

When we get to optimization, the -s flag makes a huge difference when linking
