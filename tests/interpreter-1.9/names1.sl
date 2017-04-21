let c = 123
in
  loop a = 1 and
       b = 0
  in
     if (a == 2) then
       b
     else
       let c = 456
       in
         recur ((a+1)) (b)
       end
     end
  end
end
