let lotsofargs1 a b c d e f g h i j =
  a + b + c + d + e + f + g + h + i + j
end

let lotsofargs2 a b c d e f g h i j =
  lotsofargs1
    (lotsofargs1 (a) (b) (c) (d) (e) (f) (g) (h) (i) (j))
    (lotsofargs1 (a) (b) (c) (d) (e) (f) (g) (h) (i) (j))
    (lotsofargs1 (a) (b) (c) (d) (e) (f) (g) (h) (i) (j))
    (lotsofargs1 (a) (b) (c) (d) (e) (f) (g) (h) (i) (j))
    (lotsofargs1 (a) (b) (c) (d) (e) (f) (g) (h) (i) (j))
    (lotsofargs1 (a) (b) (c) (d) (e) (f) (g) (h) (i) (j))
    (lotsofargs1 (a) (b) (c) (d) (e) (f) (g) (h) (i) (j))
    (lotsofargs1 (a) (b) (c) (d) (e) (f) (g) (h) (i) (j))
    (lotsofargs1 (a) (b) (c) (d) (e) (f) (g) (h) (i) (j))
    (lotsofargs1 (a) (b) (c) (d) (e) (f) (g) (h) (i) (j))
end

let main x =
  lotsofargs2 (x) (x) (x) (x) (x) (x) (x) (x) (x) (x)
end
