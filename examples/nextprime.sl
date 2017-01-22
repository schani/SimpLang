let bitset x i =
  loop x = x and
       i = i in
    if i < 63 then
      recur (x*2) (i+1)
    else
      x < 0
    end
  end
end

let shiftl x a =
  loop x = x and
       i = 0 in
    if i < a then
      recur (x*2) (i+1)
    else
      x
    end
  end
end

let shiftr x a =
  loop r = 0 and
       i = 0 in
    if a+i < 64 then
      recur (r + shiftl (bitset (x) (a+i)) (i)) (i+1)
    else
      r
    end
  end
end

let leadingzeros x =
  if x == 0 then
    64
  else
    if x < 0 then
      0
    else
      1 + (leadingzeros (x*2))
    end
  end
end

let sign x =
  if x == 0 then
    0
  else
    if x < 0 then
      -1
    else
      1
    end
  end
end

let div x y =
  let multiplier = sign (x) * sign (y) and
      x = x * sign (x) and
      y = y * sign (y) and
      lz = leadingzeros (y) in
    loop x = x and
         r = 0 and
         i = lz+-1 in
      if i < 0 then
        r * multiplier
      else
        let sy = shiftl (y) (i) in
          if !(x < sy) then
            recur (x+-sy) (r + (shiftl (1) (i))) (i+-1)
          else
            recur (x) (r) (i+-1)
          end
        end
      end
    end
  end
end

let rem x y =
  let quot = div (x) (y) in
    x + -(y * quot)
  end
end

let min a b =
  if a < b then
    a
  else
    b
  end
end

let sqrt x =
  loop l = 1 and
       h = min (x) (3037000499) in
    if l == h then
      l
    else
      let m = l + shiftr (h+-l+1) (1) in
        if x < m*m then
          recur (l) (m+-1)
        else
          recur (m) (h)
        end
      end
    end
  end
end

let isprime n =
  let u = sqrt (n) + 1 in
    loop i = 2 in
      if i < u then
        if rem (n) (i) == 0 then
          0
        else
          recur (i+1)
        end
      else
        1
      end
    end
  end
end

let nextprime n =
  loop n = n + 1 in
    if isprime (n) then
      n
    else
      recur (n+1)
    end
  end
end

let main x =
  nextprime (x)
end
