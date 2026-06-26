# Aji

Aji is an interpreter (programming language).

```
{@
	/* Bubble sort code.
	 * Written by Aji
	 */
	v = [3, 1, 2, 4, 5]

	for i = 0; i < len(v); i += 1:
		for j = i; j < len(v); j += 1:
			if v[i] > v[j]:
				// Swap
				tmp = v[i]
				v[i] = v[j]
				v[j] = tmp
				break
			end
		end
	end

	puts(v)
	// [1, 2, 3, 4, 5]
@}
```

## Functions

### Blocks

```
{@
    // This is code-block
@}

    This is text-block

{: "This is reference-block" :}
```

### if statement

```
{@
    /**
     * Flag test
     *
     * @param {bool} flag
     */
    def checkFlag(flag):
        if flag == 0:
            if not flag:
                puts("flag is 0")
            elif flag:
                puts("flag is", flag)
            end
        elif flag == 1:
            puts("flag is 1")
        else:
            puts("invalid flag")
        end
    end

    checkFlag(0)
    checkFlag(1)
    checkFlag(2)

    if 0: @}
        That's true!
{@
    elif 1: @}
        That's true too!
{@
    else: @}
        Else!
{@
    end
@}
```

### for statement

```
{@
    sum = 0
    for i = 0; i < 10; i += 1:
        sum += i
    end
    puts("sum", sum)

    sum = 0
    for i < 4:
        sum += i
    end
    puts("sum", sum)

    i = 0
    for:
        if i >= 4:
            break
        end
        i += 1
    end
    puts("i", i)
@}
```

### Exception

```
{@
	try:
		nil += 1
	catch Exception as e:
		puts(e.what())
	end
@}
```

```
Stack trace:
    TypeError: (unknown module): 3: Invalid left hand operand (0).

                nil += 1
        ^
Failed to compile from stdin.
```

### Struct

```
{@
	struct Animal:
		age = 10
		name = "Poti"

		def new(age, name):
			return Animal(age, name)
		end

		met show(self):
			puts(self.age, self.name)
		end
	end

	animal = Animal.new(20, "Tama")
	animal.show() // 20 Tama
@}
```

### Enum

```
{@
	enum Nums:
		Zero
		One
		Two
	end

	puts(Nums.Zero) // 0
	puts(Nums.One) // 1
	puts(Nums.Two) // 2
@}
```

### Function

```
def func(arg, arg2):
	puts(arg, arg2)
end

func(1, 2) // 1 2
```

### Block and inject

```
{@
	def base():
		block head:
			puts("Head")
		end

		block content:
		end
	end

	def derived() extends base:
		inject content:
			puts("Content")
		end
		super()
	end

	derived()
@}
```

### Pipe expression

```
{@
	i = 1 # $r * 2 # puts($r)
	// 2
@}
```

## Build and install

You can build Aji with the following commands:

```sh
$ make clean
$ make init
$ make
$ ./build/aji -h
$ ./build/aji ./build/samples/hello-world.aji
Hello, World!
```
