    loop n = 10 and
         fac = 1
	in
		if n == 1 then
			fac
		else
			recur (n+-1) (fac*n)
		end
	end
