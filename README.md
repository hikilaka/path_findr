# path_findr

`path_findr` is an implementation of [Dijkstra's algorithm](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm), finding the most 'cost effective' path from point A to B.

### Compiling 
Using [CMake](https://cmake.org/), compiling is easy. `path_findr` only relies on the C99 standard.
```sh
$ git clone https://github.com/hikilaka/path_findr
$ cd path_findr
$ mkdir build && cd build
$ cmake ../
$ make
```

### Usage
After compiling `path_findr`, you will find that it accepts two arguments from the command line.
These are:
 - `in_map_file` -- the input map file that `path_findr` will read
  - `out_map_file` -- the output map file which `path_findr` will write to (this file does not need to exist).
  Example:
  ```sh
  $ ./path_findr map.txt map_out.txt
  ```
  `map_out.txt` will now contain the calculated path from A to B.

### License
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see http://www.gnu.org/licenses/.

