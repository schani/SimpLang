loop a = 5 and
     b = 0
in
     if (a == 0) then
         b
     else
         recur ((a+-1)) ((b +
			    loop x = 6 and
				 y = 0
			    in
				 if (x == 0) then
				     y
				 else
				     recur ((x+-1)) ((y+1))
				 end
			    end))
     end
end
