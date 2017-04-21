let c = 123
in
  loop a = 1 and
       b = 0
  in
     if (a == 2) then
       c
     else
       let c = 456
       in
         recur ((a+1)) (c)
       end
     end
  end
end
