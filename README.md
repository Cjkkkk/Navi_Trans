# Navi_Trans
A simple template based tool for serializing structured data.

## Motivation
* learn c++ template
* learn c++ traits
* learn serialization

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