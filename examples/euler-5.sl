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

let augment x y factor =
  loop x = x and
       y = y and
       r = x in
    if rem (y) (factor) == 0 then
      if rem (x) (factor) == 0 then
        recur (div (x) (factor)) (div (y) (factor)) (r)
      else
        recur (x) (div (y) (factor)) (r * factor)
      end
    else
      r
    end
  end
end

let incorporate x y =
  loop x = x and
       factor = 2 in
    if y < factor then
      x
    else
      recur (augment (x) (y) (factor)) (factor + 1)
    end
  end
end

let main max =
  loop x = 1 and
       i = 2 in
    if max < i then
      x
    else
      recur (incorporate (x) (i)) (i + 1)
    end
  end
end
