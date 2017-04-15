    loop a = 100 and
		 b = 0
	in
	    if (a == 0) then
	        b
		else
			recur ((a+-1)) ((b+1))
		end
	end
