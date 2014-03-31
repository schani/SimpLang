def fac n =
	loop acc = 1 and
		 i = 2
	in
		if n < i then
		acc
	else
			recur (acc * i) (i + 1)
		end
	end
end

def main n =
	fac n
end
