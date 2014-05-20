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
  (0 < x) +- (x < 0)
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

let main x =
  loop x = x and
       i = 2 in
    if i == x then
      i
    else
      if rem (x) (i) == 0 then
        recur (div (x) (i)) (i)
      else
        recur (x) (i + 1)
      end
    end
  end
end
