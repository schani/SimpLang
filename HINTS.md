# Writing a parser

    AST
	ReadTopLevel () {
		var token = ReadToken ();
		if (token != TokenDef)
			throw new SyntaxError ();
		var name = ReadToken ();
		if (token != TokenIdentifier)
			throw new SyntaxErrro ();
	}
	
	AST
	ReadExpression () {
		var token = ReadToken ();
		switch (token) {
			case TokenOpenParen:
			;
			case TokenIdentifier:
			;
			case TokenLoop:
			;
			case TokenIf:
				var condition = ReadExpression ();
		}
	}
