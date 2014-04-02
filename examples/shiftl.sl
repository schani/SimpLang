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

let main x a =
  shiftl (x) (a)
end
