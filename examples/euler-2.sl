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

let iseven x =
  ! bitset (x) (0)
end

let main limit =
  loop sum = 0 and
       f1 = 0 and
       f2 = 1 in
    if limit < f2 then
      sum
    else
      let delta = if iseven (f2) then f2 else 0 end and
          sum = sum + delta in
        recur (sum) (f2) (f1 + f2)
      end
    end
  end
end
