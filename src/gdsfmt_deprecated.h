// ************************************************************************ //
// ************************************************************************ //
// ************************************************************************ //

/// return true, if Obj is a logical object in R
COREARRAY_DLL_EXPORT bool gds_Is_R_Logical(CdGDSObj &Obj)
{
	return Obj.Attribute().HasName(UTF7("R.logical"));
}


/// return true, if Obj is a factor variable
COREARRAY_DLL_EXPORT bool gds_Is_R_Factor(CdGDSObj &Obj)
{
	if (Obj.Attribute().HasName(UTF7("R.class")) &&
		Obj.Attribute().HasName(UTF7("R.levels")))
	{
		return (Obj.Attribute()[UTF7("R.class")].GetStr8() == "factor");
	} else
		return false;
}


/// return 1, if Obj is a factor object in R; otherwise return 0
COREARRAY_DLL_EXPORT int gds_Set_If_R_Factor(CdGDSObj &Obj, SEXP val)
{
	int nProtected = 0;

	if (Obj.Attribute().HasName(UTF7("R.class")) &&
		Obj.Attribute().HasName(UTF7("R.levels")))
	{
		if (Obj.Attribute()[UTF7("R.class")].GetStr8() == "factor")
		{
			if (Obj.Attribute()[UTF7("R.levels")].IsArray())
			{
				const TdsAny *p = Obj.Attribute()[UTF7("R.levels")].GetArray();
				C_UInt32 L = Obj.Attribute()[UTF7("R.levels")].GetArrayLength();

				SEXP levels;
				PROTECT(levels = NEW_CHARACTER(L));
				nProtected ++;
				for (C_UInt32 i=0; i < L; i++)
				{
					SET_STRING_ELT(levels, i, mkChar(p[i].
						GetStr8().c_str()));
				}

				SET_LEVELS(val, levels);
				SET_CLASS(val, mkString("factor"));
			} else if (Obj.Attribute()[UTF7("R.levels")].IsString())
			{
				SEXP levels;
				PROTECT(levels = NEW_CHARACTER(1));
				nProtected ++;
				SET_STRING_ELT(levels, 0, mkChar(Obj.Attribute()
					[UTF7("R.levels")].GetStr8().c_str()));

				SET_LEVELS(val, levels);
				SET_CLASS(val, mkString("factor"));
			}
		}
	}

	// output
	return nProtected;
}


/// return an R data object
/** \param Obj       [in] a list of objects of 'gdsn' class
 *  \param Start     [in] could be NULL
 *  \param Length    [in] could be NULL
**/
COREARRAY_DLL_EXPORT SEXP gds_Read_SEXP(CdSequenceX *Obj, C_Int32 const* Start,
	C_Int32 const* Length, const C_BOOL *const Selection[])
{
	try {
		SEXP rv_ans = R_NilValue;

		if (Obj->DimCnt() > 0)
		{
			int nProtected = 0;

			CdSequenceX::TSeqDimBuf St, Cnt;
			if (Start == NULL)
			{
				memset(St, 0, sizeof(St));
				Start = St;
			}
			if (Length == NULL)
			{
				Obj->GetDimLen(Cnt);
				Length = Cnt;
			}

			CdSequenceX::TSeqDimBuf ValidCnt;
			Obj->GetInfoSelection(Start, Length, Selection, NULL, NULL, ValidCnt);

			C_Int64 TotalCount = 1;
			for (int i=0; i < Obj->DimCnt(); i++)
				TotalCount *= ValidCnt[i];

			if (TotalCount > 0)
			{
				void *buffer;
				C_SVType SV;
				if (COREARRAY_SV_INTEGER(Obj->SVType()))
				{
					if (gds_Is_R_Logical(*Obj))
					{
						PROTECT(rv_ans = NEW_LOGICAL(TotalCount));
						buffer = LOGICAL(rv_ans);
					} else {
						PROTECT(rv_ans = NEW_INTEGER(TotalCount));
						nProtected += gds_Set_If_R_Factor(*Obj, rv_ans);
						buffer = INTEGER(rv_ans);
					}
					SV = svInt32;
				} else if (COREARRAY_SV_FLOAT(Obj->SVType()))
				{
					PROTECT(rv_ans = NEW_NUMERIC(TotalCount));
					buffer = REAL(rv_ans);
					SV = svFloat64;
				} else if (COREARRAY_SV_STRING(Obj->SVType()))
				{
					PROTECT(rv_ans = NEW_CHARACTER(TotalCount));
					buffer = NULL;
					SV = svStrUTF8;
				} else
					throw ErrGDSFmt("Invalid SVType of array-oriented object.");
				nProtected ++;

				// initialize dimension
				if (Obj->DimCnt() > 1)
				{
					SEXP dim;
					PROTECT(dim = NEW_INTEGER(Obj->DimCnt()));
					nProtected ++;
					int I = 0;
					for (int k=Obj->DimCnt()-1; k >= 0; k--)
						INTEGER(dim)[ I++ ] = ValidCnt[k];
					SET_DIM(rv_ans, dim);
				}

				if (buffer != NULL)
				{
					if (!Selection)
						Obj->rData(Start, Length, buffer, SV);
					else
						Obj->rDataEx(Start, Length, Selection, buffer, SV);
				} else {
					vector<string> strbuf(TotalCount);
					if (!Selection)
						Obj->rData(Start, Length, &strbuf[0], SV);
					else
						Obj->rDataEx(Start, Length, Selection, &strbuf[0], SV);
					for (size_t i=0; i < strbuf.size(); i++)
						SET_STRING_ELT(rv_ans, i, mkChar(strbuf[i].c_str()));
				}
			} else {
				if (COREARRAY_SV_INTEGER(Obj->SVType()))
				{
					if (gds_Is_R_Logical(*Obj))
					{
						PROTECT(rv_ans = NEW_LOGICAL(0));
					} else {
						PROTECT(rv_ans = NEW_INTEGER(0));
						nProtected += gds_Set_If_R_Factor(*Obj, rv_ans);
					}
				} else if (COREARRAY_SV_FLOAT(Obj->SVType()))
				{
					PROTECT(rv_ans = NEW_NUMERIC(0));
				} else if (COREARRAY_SV_STRING(Obj->SVType()))
				{
					PROTECT(rv_ans = NEW_CHARACTER(0));
				} else
					throw ErrGDSFmt("Invalid SVType of array-oriented object.");
				nProtected ++;
			}

			// unprotect the object
			if (nProtected > 0)
				UNPROTECT(nProtected);
		}

		return rv_ans;
	}
	catch (ErrAllocRead &E) {
		error("Writable only and please call 'readmode.gdsn' before reading.");
	}
	catch (exception &E) {
		error(E.what());
	}
	catch (const char *E) {
		error(E);
	}
	return R_NilValue;
}
