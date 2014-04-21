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

let numdigits x =
  loop max = 1 and
       num = 0 in
    if x < max then
      num
    else
      recur (max * 10) (num + 1)
    end
  end
end

let nthdigit x n =
  loop x = x and
       n = n in
    if n == 0 then
      rem (x) (10)
    else
      recur (div (x) (10)) (n + -1)
    end
  end
end

let ispalindrome x =
  let n = numdigits (x) in
    loop i = 0 in
      if i < n then
        if ! (nthdigit (x) (i) == nthdigit (x) (n + -i + -1)) then
          0
        else
          recur (i + 1)
        end
      else
        1
      end
    end
  end
end

let largestpalindrome max =
  loop i = max in
    let found = loop i = i and
                     j = max in
                  if max < i then
                    -1
                  else
                    if ispalindrome (i*j) then
                      (i*j)
                    else
                      recur (i+1) (j+-1)
                    end
                  end
                end in
      if found < 0 then
        recur (i+-1)
      else
        found
      end
    end
  end
end

let main max =
  largestpalindrome (max)
end
