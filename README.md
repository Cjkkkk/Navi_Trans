# Navi_Trans
A simple template based tool for serializing structured data.

## motivation
* learn c++ template
* learn c++ traits
* learn serialization

## support wire type
* varint
* 32 bits
* 64 bits
* length delimited (partial)

## compile
```
g++ compiler.cpp -o compiler
```

## example
```bash
cd example
../compiler .proto.json .
g++ main.cpp -I ../ -o main
./main
```

## TODO
* test
* finish length delimited
* compiler for generating class file