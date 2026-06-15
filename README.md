# Aji

Aji is an interpreter (programming language).

Around 2019, I was developing a snippet manager called Cap. Over time, Cap came to implement a template language for snippets, and that language became the predecessor of Aji. Around 2020, the language was forked from Cap and became an independent project. After various twists and turns, it eventually became Aji.

If you would like to know what kind of language Aji is, please take a look at the sample code under `samples/`.

You can build Aji with the following commands:

```sh
$ make clean
$ make init
$ make
$ ./build/aji -h
$ ./build/aji ./build/samples/hello-world.aji
Hello, World!
```

2026-05-15