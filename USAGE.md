# QCVM Usage Guide

This guide walks the reader through the basics of using the QCVM.

## Usage Examples

- [Minimal test application](./examples/qcvm_test/main.c)
- [QCPONG: Small game engine using SDL2 & QCVM](./examples/qcpong/main.c)

## Getting Started

An absolutely minimal program looks like this:

```c
int main(int argc, char **argv)
{
	// load qc module
	qcvm_t *qcvm = qcvm_from_file("progs.dat");

	// check validity
	if (qcvm == NULL)
	{
		printf("failed to load \"progs.dat\"\n");
		return 1;
	}

	// get handle of function "main()"
	int function_main = qcvm_find_function(qcvm, "main");

	// check validity
	if (function_main < 1)
	{
		printf("failed to find function \"main()\" in \"progs.dat\"\n");
		return 1;
	}

	// run function "main()"
	qcvm_run(qcvm, function_main);

	// destroy qcvm context
	qcvm_free(qcvm);

	// return success
	return 0;
}
```

This will load the QC module into memory, execute a function named "main", and then clean up the memory associated with the QCVM.

## Setting Input Parameters

Let's say you have a function in QuakeC which is formatted like this:

```c
void myfunction(float f, vector v, string s);
```

If you wanted to call this function from C while also passing in those two arguments, you'd format it like this in your C program:

```c
// set parameter 0 to "123"
qcvm_set_parm_float(qcvm, 0, 123);

// set parameter 1 to "[1, 2, 3]"
qcvm_set_parm_vector(qcvm, 1, 1, 2, 3);

// set parameter 2 to "hello, world!"
qcvm_set_parm_string(qcvm, 2, "hello, world!");

// execute function
qcvm_run(qcvm, function_main);
```

## Exporting Functions From C To QuakeC

The basic format of a C -> QuakeC function is this:

```c
void my_export_func(qcvm_t *qcvm)
{
	// do something in C
}
```

### Adding Your Export

To add your export to the list of functions callable by QuakeC, you can do this:

```c

// create a structure in memory with the relevent information
qcvm_export_t my_export_struct =
{
	.func = my_export_func,
	.name = "my_export",
	.type = QCVM_VOID,
	.argc = 0
};

// add it to the qcvm memory
int my_export_id = qcvm_add_export(qcvm, &my_export_struct);
```

`my_export_id` will be the ID of the export you just added.

### Exposing your export to QuakeC

This cannot be done at runtime, but this function will automatically write a QuakeC file with your exports:

```c
qcvm_dump_exports(qcvm, "exports.qc");
```

### Getting Parameters

If your exported function has parameters, you will get them this way:

```c
void my_export_func(qcvm_t *qcvm)
{
	// get parm 0 as float
	float f = qcvm_get_parm_float(qcvm, 0);

	// get parm 1 as vector
	qcvm_vec3_t v = qcvm_get_parm_vector(qcvm, 1);

	// get parm 2 as string
	const char *s = qcvm_get_parm_string(qcvm, 2);

	// do stuff with parameters
}
```

### Returning Values

To return a value back to QuakeC, you can do it this way:

```c
void my_export_func(qcvm_t *qcvm)
{
	// get first parm
	float f = qcvm_get_parm_float(qcvm, 0);

	// return f * f
	qcvm_return_float(qcvm, f * f);
}
```

## Setting Global Variables

QuakeC can have global variables that are both readable and writeable. To write into a global field, you can do it like this:

```c
// get global handle
int my_global_float = qcvm_find_global(qcvm, "my_global_float");

// set global
qcvm_set_global_float(qcvm, my_global_float, 123);
```

A vector global field:

```c
// get global handle
int my_global_vector = qcvm_find_global(qcvm, "my_global_vector");

// set global
qcvm_set_global_vector(qcvm, my_global_vector, 1, 2, 3);
```
