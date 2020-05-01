/*
 * ERMInterpreter.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "ERMInterpreter.h"

#include <cctype>

namespace spirit = boost::spirit;
using ::scripting::ContextBase;
using namespace ::VERMInterpreter;

typedef int TUnusedType;

namespace ERMConverter
{
	//console printer
	using namespace ERM;

	struct Variable
	{
		std::string name = "";
		std::string macro = "";
		int index = 0;

		Variable(const std::string & name_, int index_)
		{
			name = name_;
			index = index_;
		}

		Variable(const std::string & macro_)
		{
			macro = macro_;
		}

		bool isEmpty() const
		{
			return name == "" && macro == "";
		}
	};

	struct LVL2IexpToVar : boost::static_visitor<Variable>
	{
		LVL2IexpToVar() = default;

		Variable operator()(const TVarExpNotMacro & val) const
		{
			if(val.val.is_initialized())
				return Variable(val.varsym, val.val.get());
			else
				return Variable(val.varsym, 0);
		}

		Variable operator()(const TMacroUsage & val) const
		{
			return Variable(val.macro);
		}
	};

	struct LVL1IexpToVar : boost::static_visitor<Variable>
	{
		LVL1IexpToVar() = default;

		Variable operator()(int const & constant) const
		{
			return Variable("", constant);
		}

		Variable operator()(const TVarExp & var) const
		{
			return boost::apply_visitor(LVL2IexpToVar(), var);
		}
	};

	struct LVL2Iexp : boost::static_visitor<std::string>
	{
		LVL2Iexp() = default;

		std::string operator()(const TVarExpNotMacro & val) const
		{
			if(val.questionMark.is_initialized())
				throw EIexpProblem("Question marks ('?') are not allowed in getter i-expressions");

			if(val.val.is_initialized())
				return boost::to_string(boost::format("%s['%d']") % val.varsym % val.val.get());
			else
				return boost::to_string(boost::format("Q['%s']") % val.varsym);
		}

		std::string operator()(const TMacroUsage & val) const
		{
			return boost::to_string(boost::format("M['%s']") % val.macro);
		}
	};

	struct LVL1Iexp : boost::static_visitor<std::string>
	{
		LVL1Iexp() = default;

		std::string operator()(int const & constant) const
		{
			return std::to_string(constant);
		}

		std::string operator()(const TVarExp & var) const
		{
			return boost::apply_visitor(LVL2Iexp(), var);
		}
	};

	struct Condition : public boost::static_visitor<std::string>
	{
		Condition()
		{}

		std::string operator()(const TComparison & cmp) const
		{
			std::string lhs = boost::apply_visitor(LVL1Iexp(), cmp.lhs);
			std::string rhs = boost::apply_visitor(LVL1Iexp(), cmp.rhs);

			static const std::map<std::string, std::string> OPERATION =
			{
				{"<", "<"},
				{">", ">"},
				{">=", ">="},
				{"=>", ">="},
				{"<=", "<="},
				{"=<", "<="},
				{"==", "=="},
				{"<>", "~="},
				{"><", "~="},
			};

			auto sign = OPERATION.find(cmp.compSign);
			if(sign == std::end(OPERATION))
				throw EScriptExecError(std::string("Wrong comparison sign: ") + cmp.compSign);

			boost::format fmt("(%s %s %s)");
			fmt % lhs % sign->second % rhs;
			return fmt.str();
		}
		std::string operator()(int const & flag) const
		{
			return boost::to_string(boost::format("F['%d']") % flag);
		}
	};

	struct ParamIO
	{
		std::string name;
		bool isInput;
	};

	struct Converter : public boost::static_visitor<>
	{
		mutable std::ostream * out;
		Converter(std::ostream * out_)
			: out(out_)
		{}
	protected:

		void put(const std::string & text) const
		{
			(*out) << text;
		}

		void putLine(const std::string & line) const
		{
			(*out) << line << std::endl;
		}

		void endLine() const
		{
			(*out) << std::endl;
		}
	};

	struct GetBodyOption : public boost::static_visitor<std::string>
	{
		GetBodyOption()
		{}

		virtual std::string operator()(const TVarConcatString & cmp) const
		{
			throw EScriptExecError("String concatenation not allowed in this receiver");
		}
		virtual std::string operator()(const TStringConstant & cmp) const
		{
			throw EScriptExecError("String constant not allowed in this receiver");
		}
		virtual std::string operator()(const TCurriedString & cmp) const
		{
			throw EScriptExecError("Curried string not allowed in this receiver");
		}
		virtual std::string operator()(const TSemiCompare & cmp) const
		{
			throw EScriptExecError("Semi comparison not allowed in this receiver");
		}
		virtual std::string operator()(const TMacroDef & cmp) const
		{
			throw EScriptExecError("Macro definition not allowed in this receiver");
		}
		virtual std::string operator()(const TIexp & cmp) const
		{
			throw EScriptExecError("i-expression not allowed in this receiver");
		}
		virtual std::string operator()(const TVarpExp & cmp) const
		{
			throw EScriptExecError("Varp expression not allowed in this receiver");
		}
		virtual std::string operator()(const spirit::unused_type & cmp) const
		{
			throw EScriptExecError("\'Nothing\' not allowed in this receiver");
		}
	};

	struct BodyOption : public boost::static_visitor<ParamIO>
	{
		ParamIO operator()(const TVarConcatString & cmp) const
		{
			throw EScriptExecError("String concatenation not allowed in this receiver");
		}

		ParamIO operator()(const TStringConstant & cmp) const
		{
			boost::format fmt("[===[%s]===]");
			fmt % cmp.str;

			ParamIO ret;
			ret.isInput = true;
			ret.name = fmt.str();
			return ret;
		}

		ParamIO operator()(const TCurriedString & cmp) const
		{
			throw EScriptExecError("Curried string not allowed in this receiver");
		}

		ParamIO operator()(const TSemiCompare & cmp) const
		{
			throw EScriptExecError("Semi comparison not allowed in this receiver");
		}

		ParamIO operator()(const TMacroDef & cmp) const
		{
			throw EScriptExecError("Macro definition not allowed in this receiver");
		}

		ParamIO operator()(const TIexp & cmp) const
		{
			ParamIO ret;
			ret.isInput = true;
			ret.name = boost::apply_visitor(LVL1Iexp(), cmp);
			return ret;
		}

		ParamIO operator()(const TVarpExp & cmp) const
		{
			ParamIO ret;
			ret.isInput = false;

			ret.name = boost::apply_visitor(LVL2Iexp(), cmp.var);
			return ret;
		}

		ParamIO operator()(const spirit::unused_type & cmp) const
		{
			throw EScriptExecError("\'Nothing\' not allowed in this receiver");
		}
	};

	struct Receiver : public Converter
	{
		Receiver(std::ostream * out_)
			: Converter(out_)
		{}

		virtual void operator()(const TVRLogic & trig) const
		{
			throw EInterpreterError("VR logic is not allowed in this receiver!");
		}

		virtual void operator()(const TVRArithmetic & trig) const
		{
			throw EInterpreterError("VR arithmetic is not allowed in this receiver!");
		}

		virtual void operator()(const TNormalBodyOption & trig) const
		{
			throw EInterpreterError("Normal body is not allowed in this receiver!");
		}

	};

	struct GenericReceiver : public Receiver
	{
		std::string name;

		GenericReceiver(std::ostream * out_, const std::string & name_)
			: Receiver(out_),
			name(name_)
		{}

		using Receiver::operator();

		void operator()(const TNormalBodyOption & trig) const override
		{
			std::string outParams;
			std::string inParams;

			bool hasOutput = false;

			{
				std::vector<ParamIO> optionParams;

				for(auto & p : trig.params)
					optionParams.push_back(boost::apply_visitor(BodyOption(), p));

				for(auto & p : optionParams)
				{
					if(p.isInput)
					{
						if(outParams.empty())
							outParams = "_";
						else
							outParams += ", _";

						inParams += ", ";
						inParams += p.name;
					}
					else
					{
						hasOutput = true;

						if(outParams.empty())
						{
							outParams = p.name;
						}
						else
						{
							outParams += ", ";
							outParams += p.name;
						}

						inParams += ", nil";
					}
				}
			}

			boost::format callFormat;

			if(hasOutput)
			{
				callFormat.parse("%s = %s:%s(x%s)");
				callFormat % outParams;
			}
			else
			{
				callFormat.parse("%s:%s(x%s)");
			}

			callFormat % name;
			callFormat % trig.optionCode;
			callFormat % inParams;

			putLine(callFormat.str());
		}
	};

	struct FU : public Receiver
	{
		Variable v;

		FU(std::ostream * out_, const ERM::TIexp & tid)
			: Receiver(out_),
			v(boost::apply_visitor(LVL1IexpToVar(), tid))
		{
		}

		FU(std::ostream * out_)
			: Receiver(out_),
			v("", 0)
		{
		}

		using Receiver::operator();

		void operator()(const TNormalBodyOption & trig) const override
		{
			switch(trig.optionCode)
			{
			case 'E':
				{
					putLine("do return end");
				}
				break;
			case 'P':
				{
					std::vector<ParamIO> optionParams;

					for(auto & p : trig.params)
						optionParams.push_back(boost::apply_visitor(BodyOption(), p));

					auto index = 1;

					putLine("local newx = {}");

					for(auto & p : optionParams)
					{
						if(p.isInput)
						{
							boost::format fmt("newx['%d'] = %s");
							fmt % index % p.name;
							putLine(fmt.str());
						}

						index++;
					}

					boost::format callFormat("FU%d(newx)");
					callFormat % v.index;
					putLine(callFormat.str());

					index = 1;
					for(auto & p : optionParams)
					{
						if(!p.isInput)
						{
							boost::format fmt("%s = newx['%d']");
							fmt % p.name % index;
							putLine(fmt.str());
						}

						index++;
					}
				}
				break;
			default:
				throw EInterpreterError("Unknown opcode in FU receiver");
				break;
			}
		}
	};

	struct MC_S : public GetBodyOption
	{
		MC_S()
		{}

		using GetBodyOption::operator();

		std::string operator()(const TMacroDef & cmp) const override
		{
			return cmp.macro;
		}
	};

	struct MC : public Receiver
	{
		Variable v;

		MC(std::ostream * out_, const ERM::TIexp & tid)
			: Receiver(out_),
			v(boost::apply_visitor(LVL1IexpToVar(), tid))
		{
		}

		MC(std::ostream * out_)
			: Receiver(out_),
			v("", 0)
		{
		}

		using Receiver::operator();

		void operator()(const TNormalBodyOption & option) const override
		{
			switch(option.optionCode)
			{
			case 'S':
				{
					for(auto & p : option.params)
					{
						std::string macroName = boost::apply_visitor(MC_S(), p);

						boost::format callFormat;

						if(v.isEmpty())
						{
							callFormat.parse("ERM:addMacro('%s', 'v', '%s')");
							callFormat % macroName % macroName;
						}
						else
						{
							callFormat.parse("ERM:addMacro('%s', '%s', '%d')");
							callFormat % macroName % v.name % v.index;
						}

						putLine(callFormat.str());
					}
				}
				break;
			default:
				throw EInterpreterError("Unknown opcode in MC receiver");
				break;
			}
		}
	};

	struct VR_S : public GetBodyOption
	{
		VR_S()
		{}

		using GetBodyOption::operator();

		std::string operator()(const TIexp & cmp) const override
		{
			return boost::apply_visitor(LVL1Iexp(), cmp);
		}
		std::string operator()(const TStringConstant & cmp) const override
		{
			boost::format fmt("[===[%s]===]");
			fmt % cmp.str;
			return fmt.str();
		}
	};

	struct VR_H : public GetBodyOption
	{
		VR_H()
		{}

		using GetBodyOption::operator();

		std::string operator()(const TIexp & cmp) const override
		{
			Variable p = boost::apply_visitor(LVL1IexpToVar(), cmp);

			if(p.index <= 0)
				throw EScriptExecError("VR:H requires flag index");

			if(p.name != "")
				throw EScriptExecError("VR:H accept only flag index");


			boost::format fmt("'%d'");
			fmt % p.index;
			return fmt.str();
		}
	};

	struct VR : public Receiver
	{
		std::string var;//todo: remove it
		Variable v;

		VR(std::ostream * out_, const ERM::TIexp & tid)
			: Receiver(out_),
			var(boost::apply_visitor(LVL1Iexp(), tid)),
			v(boost::apply_visitor(LVL1IexpToVar(), tid))
		{
		}

		using Receiver::operator();

		void operator()(const TVRLogic & trig) const override
		{
			std::string rhs = boost::apply_visitor(LVL1Iexp(), trig.var);

			std::string opcode;

			switch (trig.opcode)
			{
			case '&':
				opcode = "bit.band";
				break;
			case '|':
				opcode = "bit.bor";
				break;
			case 'X':
				opcode = "bit.bxor";
				break;
			default:
				throw EInterpreterError("Wrong opcode in VR logic expression!");
				break;
			}

			boost::format fmt("%s = %s %s(%s, %s)");
			fmt % var % opcode % var % rhs;
			putLine(fmt.str());
		}

		void operator()(const TVRArithmetic & trig) const override
		{
			std::string rhs = boost::apply_visitor(LVL1Iexp(), trig.rhs);

			std::string opcode;

			switch (trig.opcode)
			{
			case '+':
			case '-':
			case '*':
			case '%':
				opcode = trig.opcode;
				break;
			case ':':
				opcode = "/";
				break;
			default:
				throw EInterpreterError("Wrong opcode in VR arithmetic!");
				break;
			}

			boost::format fmt("%s = %s %s %s");
			fmt % var %  var % opcode % rhs;
			putLine(fmt.str());
		}

		void operator()(const TNormalBodyOption & trig) const override
		{
			switch(trig.optionCode)
			{
			case 'C': //setting/checking v vars
				{
					if(v.index <= 0)
						throw EScriptExecError("VR:C requires indexed variable");

					std::vector<ParamIO> optionParams;

					for(auto & p : trig.params)
						optionParams.push_back(boost::apply_visitor(BodyOption(), p));

					auto index = v.index;

					for(auto & p : optionParams)
					{
						boost::format fmt;

						if(p.isInput)
						{
							fmt.parse("%s['%d'] = %s") % v.name % index % p.name;
						}
						else
						{
							fmt.parse("%s = %s['%d']") % p.name % v.name % index;
						}
						putLine(fmt.str());
						index++;
					}
				}
				break;
			case 'H': //checking if string is empty
				{
					if(trig.params.size() != 1)
						throw EScriptExecError("VR:H option takes exactly 1 parameter!");

					std::string opt = boost::apply_visitor(VR_H(), trig.params[0]);
					boost::format fmt("ERM.VR(%s):H(%s)");
					fmt % var % opt;
					putLine(fmt.str());
				}
				break;
			case 'M': //string operations
				{
					//TODO
					throw EScriptExecError("VR:M not implemented");
				}
				break;
			case 'R': //random variables
				{
					//TODO
					throw EScriptExecError("VR:R not implemented");
				}
				break;
			case 'S': //setting variable
				{
					if(trig.params.size() != 1)
						throw EScriptExecError("VR:S option takes exactly 1 parameter!");

					std::string opt = boost::apply_visitor(VR_S(), trig.params[0]);
					put(var);
					put(" = ");
					put(opt);
					endLine();
				}
				break;
			case 'T': //random variables
				{
					//TODO
					throw EScriptExecError("VR:T not implemented");
				}
				break;
			case 'U': //search for a substring
				{
					//TODO
					throw EScriptExecError("VR:U not implemented");
				}
				break;
			case 'V': //convert string to value
				{
					//TODO
					throw EScriptExecError("VR:V not implemented");
				}
				break;
			default:
				throw EScriptExecError("Wrong VR receiver option!");
				break;
			}
		}
	};


	struct ERMExp : public Converter
	{
		ERMExp(std::ostream * out_)
			: Converter(out_)
		{}

		template <typename Visitor>
		void performBody(const boost::optional<ERM::Tbody> & body, const Visitor & visitor) const
		{
			if(body.is_initialized())
			{
				ERM::Tbody bo = body.get();
				for(int g=0; g<bo.size(); ++g)
				{
					boost::apply_visitor(visitor, bo[g]);
				}
			}
		}

		void convert(const std::string & name, const boost::optional<Tidentifier> & identifier, const boost::optional<Tbody> & body) const
		{
			if(name == "VR")
			{
				if(!identifier.is_initialized())
					throw EScriptExecError("VR receiver requires arguments");

				ERM::Tidentifier tid = identifier.get();
				if(tid.size() != 1)
					throw EScriptExecError("VR receiver takes exactly 1 argument");

				performBody(body, VR(out, tid[0]));
			}
			else if(name == "FU")
			{
				if(identifier.is_initialized())
				{
					ERM::Tidentifier tid = identifier.get();

					if(tid.size() > 0)
						performBody(body, FU(out, tid[0]));
				}
				else
				{
					performBody(body, FU(out));
				}
			}
			else if(name == "DO")
			{
				//TODO: use P body option
				//TODO: pass|return parameters
				if(!identifier.is_initialized())
					throw EScriptExecError("DO receiver requires arguments");

				ERM::Tidentifier tid = identifier.get();
				if(tid.size() != 4)
					throw EScriptExecError("DO receiver takes exactly 4 arguments");

				auto funNum = boost::apply_visitor(LVL1Iexp(), tid[0]);
				auto startVal = boost::apply_visitor(LVL1Iexp(), tid[1]);
				auto stopVal = boost::apply_visitor(LVL1Iexp(), tid[2]);
				auto increment = boost::apply_visitor(LVL1Iexp(), tid[3]);

				(*out) << "for __iter = " << startVal <<", " << stopVal << "-1, " << increment << " do " << std::endl;
				(*out) << "\tlocal x = x or {}" << std::endl;
				(*out) << "\tx['16'] = __iter" << std::endl;
				(*out) << "\tFU" << funNum << "(x)" << std::endl;
				(*out) << "\t__iter = x['16']" << std::endl;
				(*out) << "end" << std::endl;
			}
			else if(name == "MC")
			{
				if(identifier.is_initialized())
				{
					ERM::Tidentifier tid = identifier.get();
					if(tid.size() != 1)
						throw EScriptExecError("MC receiver takes no more than 1 argument");

					performBody(body, MC(out, tid[0]));
				}
				else
				{
					performBody(body, MC(out));
				}
			}
			else
			{
				std::vector<std::string> identifiers;

				if(identifier.is_initialized())
				{
					for(const auto & id : identifier.get())
						identifiers.push_back(boost::apply_visitor(LVL1Iexp(), id));
				}

				putLine("do");

				std::string params;

				for(auto iter = std::begin(identifiers); iter != std::end(identifiers); ++iter)
				{
					if(!params.empty())
						params += ", ";
					params += *iter;
				}

				boost::format fmt("local %s = ERM.%s(%s)");
				fmt % name;
				fmt % name;
				fmt % params;

				putLine(fmt.str());

				performBody(body, GenericReceiver(out, name));

				putLine("end");
			}
		}

		void convertConditionInner(const Tcondition & cond, char op) const
		{
			std::string lhs = boost::apply_visitor(Condition(), cond.cond);

			if(cond.ctype != '/')
				op = cond.ctype;

			switch (op)
			{
			case '&':
				put(" and ");
				break;
			case '|':
				put(" or ");
				break;
			default:
				throw EInterpreterProblem(std::string("Wrong condition connection (") + cond.ctype + ") !");
				break;
			}

			put(lhs);

			if(cond.rhs.is_initialized())
			{
				switch (op)
				{
				case '&':
				case '|':
					break;
				default:
					throw EInterpreterProblem(std::string("Wrong condition connection (") + cond.ctype + ") !");
					break;
				}

				convertConditionInner(cond.rhs.get().get(), op);
			}
		}

		void convertCondition(const Tcondition & cond) const
		{
			//&c1/c2/c3|c4/c5/c6 -> (c1  & c2  & c3)  | c4  |  c5  | c6
			std::string lhs = boost::apply_visitor(Condition(), cond.cond);
			put("if ");
			put(lhs);

			if(cond.rhs.is_initialized())
			{
				switch (cond.ctype)
				{
				case '&':
				case '|':
					break;
				default:
					throw EInterpreterProblem(std::string("Wrong condition connection (") + cond.ctype + ") !");
					break;
				}

				convertConditionInner(cond.rhs.get().get(), cond.ctype);
			}

			putLine(" then ");
		}

		void convertReceiverOrInstruction(const boost::optional<Tcondition> & condition,
			const std::string & name,
			const boost::optional<Tidentifier> & identifier,
			const boost::optional<Tbody> & body) const
		{
			if(name=="if")
			{
				if(condition.is_initialized())
					convertCondition(condition.get());
				else
					putLine("if true then");
			}
			else if(name=="el")
			{
				putLine("else");
			}
			else if(name=="en")
			{
				putLine("end");
			}
			else
			{
				if(condition.is_initialized())
				{
					convertCondition(condition.get());
					convert(name, identifier, body);
					putLine("end");
				}
				else
				{
					convert(name, identifier, body);
				}
			}
		}

		void operator()(const Ttrigger & trig) const
		{
			throw EInterpreterError("Triggers cannot be executed!");
		}

		void operator()(const TPostTrigger & trig) const
		{
			throw EInterpreterError("Post-triggers cannot be executed!");
		}

		void operator()(const Tinstruction & trig) const
		{
			convertReceiverOrInstruction(trig.condition, trig.name, trig.identifier, boost::make_optional(trig.body));
		}

		void operator()(const Treceiver & trig) const
		{
			convertReceiverOrInstruction(trig.condition, trig.name, trig.identifier, trig.body);
		}
	};

	struct Command : public Converter
	{
		Command(std::ostream * out_)
			: Converter(out_)
		{}

		void operator()(const Tcommand & cmd) const
		{
			boost::apply_visitor(ERMExp(out), cmd.cmd);
		}
		void operator()(const std::string & comment) const
		{
			(*out) << "-- " << comment;
			endLine();
		}

		void operator()(spirit::unused_type const &) const
		{
		}
	};

	struct TLiteralEval : public boost::static_visitor<std::string>
	{

		std::string operator()(char const & val)
		{
			return "'"+ std::to_string(val) +"'";
		}
		std::string operator()(double const & val)
		{
			return std::to_string(val);
		}
		std::string operator()(int const & val)
		{
			return std::to_string(val);
		}
		std::string operator()(const std::string & val)
		{
			return "[===[" + val + "]===]";
		}
	};

	struct VOptionEval : public Converter
	{
		VOptionEval(std::ostream * out_)
			: Converter(out_)
		{}

		void operator()(VNIL const & opt) const
		{
			(*out) << "{}";
		}
		void operator()(VNode const & opt) const;

		void operator()(VSymbol const & opt) const
		{
			(*out) << "'" << opt.text << "'";
		}
		void operator()(TLiteral const & opt) const
		{
			TLiteralEval tmp;
			(*out) << boost::apply_visitor(tmp, opt);
		}
		void operator()(ERM::Tcommand const & opt) const
		{
			//this is how FP works, evaluation == producing side effects
			//TODO: can we evaluate to smth more useful?
			//???
			throw EVermScriptExecError("Using ERM options in VERM expression is not (yet) allowed");
//			boost::apply_visitor(ERMExp(out), opt.cmd);
		}
	};

	struct VOptionNodeEval : public Converter
	{
		VNode & exp;

		VOptionNodeEval(std::ostream * out_, VNode & exp_)
			: Converter(out_),
			exp(exp_)
		{}

		void operator()(VNIL const & opt) const
		{
			throw EVermScriptExecError("Nil does not evaluate to a function");
		}

		void operator()(VNode const & opt) const
		{
			VNode tmpn(exp);

			(*out) << "{";

			VOptionEval tmp(out);
			tmp(opt);

			VOptionList args = tmpn.children.cdr().getAsList();

			for(int g=0; g<args.size(); ++g)
			{
				(*out) << ", ";
				boost::apply_visitor(VOptionEval(out), args[g]);
			}

			(*out) << "}";
		}

		void operator()(VSymbol const & opt) const
		{
			VNode tmpn(exp);

			(*out) << "{" << "'"<< opt.text << "'";

			VOptionList args = tmpn.children.cdr().getAsList();

			for(int g=0; g<args.size(); ++g)
			{
				(*out) << ", ";
				boost::apply_visitor(VOptionEval(out), args[g]);

			}

			(*out) << "}";
		}
		void operator()(TLiteral const & opt) const
		{
			throw EVermScriptExecError("Literal does not evaluate to a function: "+boost::to_string(opt));
		}
		void operator()(ERM::Tcommand const & opt) const
		{
			throw EVermScriptExecError("ERM command does not evaluate to a function");
		}
	};

	void VOptionEval::operator()(VNode const& opt) const
	{
		if(!opt.children.empty())
		{
			VOption & car = const_cast<VNode&>(opt).children.car().getAsItem();

			boost::apply_visitor(VOptionNodeEval(out, const_cast<VNode&>(opt)), car);
		}
	}

	struct Line : public Converter
	{
		Line(std::ostream * out_)
			: Converter(out_)
		{}

		void operator()(TVExp const & cmd) const
		{
			put("VERM:eval");

			VNode line(cmd);

			VOptionEval eval(out);
			eval(line);


			endLine();
		}
		void operator()(TERMline const & cmd) const
		{
			boost::apply_visitor(Command(out), cmd);
		}
	};

	void convertInstructions(std::ostream & out, ERMInterpreter * owner)
	{
		out << "local function instructions()" << std::endl;

		Line lineConverter(&out);

		for(const LinePointer & lp : owner->instructions)
		{
			ERM::TLine & line = owner->retrieveLine(lp);

			boost::apply_visitor(lineConverter, line);
		}

		out << "end" << std::endl;
	}

	void convertFunctions(std::ostream & out, ERMInterpreter * owner, const std::vector<VERMInterpreter::Trigger> & triggers)
	{
		std::map<std::string, LinePointer> numToBody;

		Line lineConverter(&out);

		for(const VERMInterpreter::Trigger & trigger : triggers)
		{
			ERM::TLine & firstLine = owner->retrieveLine(trigger.line);

			const ERM::TTriggerBase & trig = ERMInterpreter::retrieveTrigger(firstLine);

			if(!trig.identifier.is_initialized())
				throw EInterpreterError("Function must have identifier");

			ERM::Tidentifier tid = trig.identifier.get();

			if(tid.size() == 0)
				throw EInterpreterError("Function must have identifier");

			std::string num = boost::apply_visitor(LVL1Iexp(), tid[0]);

			if(vstd::contains(numToBody, num))
				throw EInterpreterError("Function index duplicated: "+num);

			numToBody[num] = trigger.line;
		}

		for(const auto & p : numToBody)
		{
			std::string name = "FU"+p.first;

			out << name << " = function(x)" << std::endl;

			LinePointer lp = p.second;

			++lp;

			out << "local y = ERM.getY('" << name << "')" << std::endl;

			for(; lp.isValid(); ++lp)
			{
				ERM::TLine curLine = owner->retrieveLine(lp);
				if(owner->isATrigger(curLine))
					break;

				boost::apply_visitor(lineConverter, curLine);
			}

			out << "end" << std::endl;
		}
	}

	void convertTriggers(std::ostream & out, ERMInterpreter * owner, const VERMInterpreter::TriggerType & type, const std::vector<VERMInterpreter::Trigger> & triggers)
	{
		Line lineConverter(&out);

		for(const VERMInterpreter::Trigger & trigger : triggers)
		{
			ERM::TLine & firstLine = owner->retrieveLine(trigger.line);

			const ERM::TTriggerBase & trig = ERMInterpreter::retrieveTrigger(firstLine);

			//TODO: identifier
			//TODO: condition

			out << "ERM:addTrigger({" << std::endl;
			out << "name = '" << trig.name << "'," << std::endl;
			out << "fn = function ()" << std::endl;

			out << "local y = ERM.getY('" << trig.name  << "')" << std::endl;
			LinePointer lp = trigger.line;
			++lp;

			for(; lp.isValid(); ++lp)
			{
				ERM::TLine curLine = owner->retrieveLine(lp);
				if(owner->isATrigger(curLine))
					break;

				boost::apply_visitor(lineConverter, curLine);
			}

			out << "\tend," << std::endl;
			out << "})" << std::endl;
		}
	}
}

struct ScriptScanner : boost::static_visitor<>
{
	ERMInterpreter * interpreter;
	LinePointer lp;

	ScriptScanner(ERMInterpreter * interpr, const LinePointer & _lp) : interpreter(interpr), lp(_lp)
	{}

	void operator()(TVExp const& cmd) const
	{
		//
	}
	void operator()(TERMline const& cmd) const
	{
		if(cmd.which() == 0) //TCommand
		{
			Tcommand tcmd = boost::get<Tcommand>(cmd);
			switch (tcmd.cmd.which())
			{
			case 0: //trigger
				{
					Trigger trig;
					trig.line = lp;
					interpreter->triggers[ TriggerType(boost::get<ERM::Ttrigger>(tcmd.cmd).name) ].push_back(trig);
				}
				break;
			case 1: //instruction
				{
					interpreter->instructions.push_back(lp);
				}
				break;
			case 3: //post trigger
				{
					Trigger trig;
					trig.line = lp;
					interpreter->postTriggers[ TriggerType(boost::get<ERM::TPostTrigger>(tcmd.cmd).name) ].push_back(trig);
				}
				break;
			default:
				break;
			}
		}

	}
};


ERMInterpreter::ERMInterpreter(vstd::CLoggerBase * logger_)
	: logger(logger_)
{

}

ERMInterpreter::~ERMInterpreter()
{

}

bool ERMInterpreter::isATrigger( const ERM::TLine & line )
{
	switch(line.which())
	{
	case 0: //v-exp
		{
			TVExp vexp = boost::get<TVExp>(line);
			if(vexp.children.size() == 0)
				return false;

			switch (getExpType(vexp.children[0]))
			{
			case SYMBOL:
				{
					//TODO: what about sym modifiers?
					//TOOD: macros?
					ERM::TSymbol sym = boost::get<ERM::TSymbol>(vexp.children[0]);
					return sym.sym == triggerSymbol || sym.sym == postTriggerSymbol;
				}
				break;
			case TCMD:
				return isCMDATrigger( boost::get<ERM::Tcommand>(vexp.children[0]) );
				break;
			default:
				return false;
				break;
			}
		}
		break;
	case 1: //erm
		{
			TERMline ermline = boost::get<TERMline>(line);
			switch(ermline.which())
			{
			case 0: //tcmd
				return isCMDATrigger( boost::get<ERM::Tcommand>(ermline) );
				break;
			default:
				return false;
				break;
			}
		}
		break;
	default:
		assert(0); //it should never happen
		break;
	}
	assert(0);
	return false;
}

ERM::EVOtions ERMInterpreter::getExpType(const ERM::TVOption & opt)
{
	//MAINTENANCE: keep it correct!
	return static_cast<ERM::EVOtions>(opt.which());
}

bool ERMInterpreter::isCMDATrigger(const ERM::Tcommand & cmd)
{
	switch (cmd.cmd.which())
	{
	case 0: //trigger
	case 3: //post trigger
		return true;
		break;
	default:
		return false;
		break;
	}
}

ERM::TLine & ERMInterpreter::retrieveLine(const LinePointer & linePtr)
{
	return scripts.find(linePtr)->second;
}

ERM::TTriggerBase & ERMInterpreter::retrieveTrigger(ERM::TLine & line)
{
	if(line.which() == 1)
	{
		ERM::TERMline &tl = boost::get<ERM::TERMline>(line);
		if(tl.which() == 0)
		{
			ERM::Tcommand &tcm = boost::get<ERM::Tcommand>(tl);
			if(tcm.cmd.which() == 0)
			{
				return boost::get<ERM::Ttrigger>(tcm.cmd);
			}
			else if(tcm.cmd.which() == 3)
			{
				return boost::get<ERM::TPostTrigger>(tcm.cmd);
			}
			throw ELineProblem("Given line is not a trigger!");
		}
		throw ELineProblem("Given line is not a command!");
	}
	throw ELineProblem("Given line is not an ERM trigger!");
}

const std::string ERMInterpreter::triggerSymbol = "trigger";
const std::string ERMInterpreter::postTriggerSymbol = "postTrigger";
const std::string ERMInterpreter::defunSymbol = "defun";

std::string ERMInterpreter::loadScript(const std::string & name, const std::string & source)
{
	CERMPreprocessor preproc(source);

	const bool isVERM = preproc.version == CERMPreprocessor::Version::VERM;

	ERMParser ep;

	std::vector<LineInfo> buf = ep.parseFile(preproc);

	for(int g=0; g<buf.size(); ++g)
		scripts[LinePointer(static_cast<int>(buf.size()), g, buf[g].realLineNum)] = buf[g].tl;

	for(auto p : scripts)
		boost::apply_visitor(ScriptScanner(this, p.first), p.second);

	std::stringstream out;

	out << "local ERM = require(\"core:erm\")" << std::endl;

	if(isVERM)
	{
		out << "local VERM = require(\"core:verm\")" << std::endl;
	}

	out << "local v = ERM.v" << std::endl;
	out << "local z = ERM.z" << std::endl;
	out << "local F = ERM.F" << std::endl;
	out << "local M = ERM.M" << std::endl;
	out << "local Q = ERM.Q" << std::endl;

	ERMConverter::convertInstructions(out, this);

	for(const auto & p : triggers)
	{
		const VERMInterpreter::TriggerType & tt = p.first;

		if(tt.type == VERMInterpreter::TriggerType::FU)
		{
			ERMConverter::convertFunctions(out, this, p.second);
		}
		else
		{
			ERMConverter::convertTriggers(out, this, tt, p.second);
		}
	}

	for(const auto & p : postTriggers)
		;//TODO:postTriggers

	out << "ERM:callInstructions(instructions)" << std::endl;

	return out.str();
}

//struct _SbackquoteEval : boost::static_visitor<VOption>
//{
//	ERMInterpreter * interp;
//
//	_SbackquoteEval(ERMInterpreter * interp_)
//		: interp(interp_)
//	{}
//
//	VOption operator()(VNIL const& opt) const
//	{
//		return opt;
//	}
//	VOption operator()(VNode const& opt) const
//	{
//		VNode ret = opt;
//		if(opt.children.size() == 2)
//		{
//			VOption fo = opt.children[0];
//			if(isA<VSymbol>(fo))
//			{
//				if(getAs<VSymbol>(fo).text == "comma")
//				{
//					return interp->eval(opt.children[1]);
//				}
//			}
//		}
//		for(int g=0; g<opt.children.size(); ++g)
//		{
//			ret.children[g] = boost::apply_visitor(_SbackquoteEval(interp), ret.children[g]);
//		}
//		return ret;
//	}
//	VOption operator()(VSymbol const& opt) const
//	{
//		return opt;
//	}
//	VOption operator()(TLiteral const& opt) const
//	{
//		return opt;
//	}
//	VOption operator()(ERM::Tcommand const& opt) const
//	{
//		return opt;
//	}

//};


namespace VERMInterpreter
{
	VOption convertToVOption(const ERM::TVOption & tvo)
	{
		return boost::apply_visitor(OptionConverterVisitor(), tvo);
	}

	VNode::VNode( const ERM::TVExp & exp )
	{
		for(int i=0; i<exp.children.size(); ++i)
		{
			children.push_back(convertToVOption(exp.children[i]));
		}
		processModifierList(exp.modifier, false);
	}

	VNode::VNode( const VOption & first, const VOptionList & rest ) /*merges given arguments into [a, rest] */
	{
		setVnode(first, rest);
	}

	VNode::VNode( const VOptionList & cdren ) : children(cdren)
	{}

	VNode::VNode( const ERM::TSymbol & sym )
	{
		children.car() = VSymbol(sym.sym);
		processModifierList(sym.symModifier, true);
	}

	void VNode::setVnode( const VOption & first, const VOptionList & rest )
	{
		children.car() = first;
		children.cdr() = rest;
	}

	void VNode::processModifierList( const std::vector<TVModifier> & modifierList, bool asSymbol )
	{
		for(int g=0; g<modifierList.size(); ++g)
		{
			if(asSymbol)
			{
				children.resize(children.size()+1);
				for(int i=children.size()-1; i >0; i--)
				{
					children[i] = children[i-1];
				}
			}
			else
			{
				children.cdr() = VNode(children);
			}

			if(modifierList[g] == "`")
			{
				children.car() = VSymbol("backquote");
			}
			else if(modifierList[g] == ",!")
			{
				children.car() = VSymbol("comma-unlist");
			}
			else if(modifierList[g] == ",")
			{
				children.car() = VSymbol("comma");
			}
			else if(modifierList[g] == "#'")
			{
				children.car() = VSymbol("get-func");
			}
			else if(modifierList[g] == "'")
			{
				children.car() = VSymbol("quote");
			}
			else
				throw EInterpreterError("Incorrect value of modifier!");
		}
	}

	VermTreeIterator & VermTreeIterator::operator=( const VOption & opt )
	{
		switch (state)
		{
		case CAR:
			if(parent->size() <= basePos)
				parent->push_back(opt);
			else
				(*parent)[basePos] = opt;
			break;
		case NORM:
			parent->resize(basePos+1);
			(*parent)[basePos] = opt;
			break;
		default://should never happen
			break;
		}
		return *this;
	}

	VermTreeIterator & VermTreeIterator::operator=( const std::vector<VOption> & opt )
	{
		switch (state)
		{
		case CAR:
			//TODO: implement me
			break;
		case NORM:
			parent->resize(basePos+1);
			parent->insert(parent->begin()+basePos, opt.begin(), opt.end());
			break;
		default://should never happen
			break;
		}
		return *this;
	}
	VermTreeIterator & VermTreeIterator::operator=( const VOptionList & opt )
	{
		return *this = opt;
	}
	VOption & VermTreeIterator::getAsItem()
	{
		if(state == CAR)
			return (*parent)[basePos];
		else
			throw EInterpreterError("iterator is not in car state, cannot get as list");
	}
	VermTreeIterator VermTreeIterator::getAsCDR()
	{
		VermTreeIterator ret = *this;
		ret.basePos++;
		return ret;
	}
	VOption & VermTreeIterator::getIth( int i )
	{
		return (*parent)[basePos + i];
	}
	size_t VermTreeIterator::size() const
	{
		return parent->size() - basePos;
	}

	VERMInterpreter::VOptionList VermTreeIterator::getAsList()
	{
		VOptionList ret;
		for(int g = basePos; g<parent->size(); ++g)
		{
			ret.push_back((*parent)[g]);
		}
		return ret;
	}

	VOption OptionConverterVisitor::operator()( ERM::TVExp const& cmd ) const
	{
		return VNode(cmd);
	}
	VOption OptionConverterVisitor::operator()( ERM::TSymbol const& cmd ) const
	{
		if(cmd.symModifier.size() == 0)
			return VSymbol(cmd.sym);
		else
			return VNode(cmd);
	}
	VOption OptionConverterVisitor::operator()( char const& cmd ) const
	{
		return TLiteral(cmd);
	}
	VOption OptionConverterVisitor::operator()( double const& cmd ) const
	{
		return TLiteral(cmd);
	}
	VOption OptionConverterVisitor::operator()(int const& cmd) const
	{
		return TLiteral(cmd);
	}
	VOption OptionConverterVisitor::operator()(ERM::Tcommand const& cmd) const
	{
		return cmd;
	}
	VOption OptionConverterVisitor::operator()( ERM::TStringConstant const& cmd ) const
	{
		return TLiteral(cmd.str);
	}

	bool VOptionList::isNil() const
	{
		return size() == 0;
	}

	VermTreeIterator VOptionList::cdr()
	{
		VermTreeIterator ret(*this);
		ret.basePos = 1;
		return ret;
	}

	VermTreeIterator VOptionList::car()
	{
		VermTreeIterator ret(*this);
		ret.state = VermTreeIterator::CAR;
		return ret;
	}

}
