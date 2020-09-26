# Navi_Trans
A simple template based tool for serializing structured data.

## Motivation
* learn c++ template
* learn c++ traits
* learn serialization

# Design
* use header only style.
* use type trait to track all memebers of class (reflection), which does not require users to modify class definition to enable easy third party class serialization.
* use tuple + partial specialization + recursive to expand varidic types.
* use struct partial specialization + static method to achieve function partial specialization.
* Fvck template.

## Support wire type
* varint
* 32 bits
* 64 bits
* length delimited

## Message encoding format
```
|---------------|-----------------------------------
| (4)           | (4)   (? )   | (4)   (?)    | ...
| payload_size  | key1 value1  | key2 value2  | ...
| --------------|------------------------------------
```

## Example
```bash
cd example/
g++ main.cpp -I ../include
```

## TODO
* test