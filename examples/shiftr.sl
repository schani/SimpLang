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

let main x a =
  shiftr (x) (a)
end
