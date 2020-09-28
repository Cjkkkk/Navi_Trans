# Navi_Trans
A simple template based tool for serializing structured data.

## Motivation
* learn c++ template
* learn c++ traits
* learn serialization

## Design
* use header only style.
* use type trait to track all memebers of class (reflection), which does not require users to modify class definition to enable easy third party class serialization.
* use tuple + partial specialization + recursive to expand varidic types.
* use struct partial specialization + static method to achieve function partial specialization.
* Fvck template.

## Support encoding type
* varint
* fixed length
* length delimited

## Message encoding format
```
|---------------|-----------------------------------
| (4)           | (4)   (? )   | (4)   (?)    | ...
| payload_size  | key1 value1  | key2 value2  | ...
| --------------|------------------------------------
```

## supported type
```
varint:
enum


fixed length:
bool
char
int8_t
uint8_t
int16_t
uint16_t
int32_t
uint32_t
int64_t
uint64_t
float
double


length delimited:
user defined class
array
std::vector
```


## Example
```bash
cd example/
g++ simple.cpp -I ../include
```

## TODO
* test