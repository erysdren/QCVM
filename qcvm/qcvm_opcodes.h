/*******************************************************************************
 *
 * MIT License
 *
 * Copyright (c) 2023-2025 erysdren (it/its)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ******************************************************************************/

/* 
 * vanilla opcodes
 */

QCVM_OPCODE(DONE)
QCVM_OPCODE(MUL_F)
QCVM_OPCODE(MUL_V)
QCVM_OPCODE(MUL_FV)
QCVM_OPCODE(MUL_VF)
QCVM_OPCODE(DIV_F)
QCVM_OPCODE(ADD_F)
QCVM_OPCODE(ADD_V)
QCVM_OPCODE(SUB_F)
QCVM_OPCODE(SUB_V)
QCVM_OPCODE(EQ_F)
QCVM_OPCODE(EQ_V)
QCVM_OPCODE(EQ_S)
QCVM_OPCODE(EQ_E)
QCVM_OPCODE(EQ_FNC)
QCVM_OPCODE(NE_F)
QCVM_OPCODE(NE_V)
QCVM_OPCODE(NE_S)
QCVM_OPCODE(NE_E)
QCVM_OPCODE(NE_FNC)
QCVM_OPCODE(LE)
QCVM_OPCODE(GE)
QCVM_OPCODE(LT)
QCVM_OPCODE(GT)
QCVM_OPCODE(LOAD_F)
QCVM_OPCODE(LOAD_V)
QCVM_OPCODE(LOAD_S)
QCVM_OPCODE(LOAD_ENT)
QCVM_OPCODE(LOAD_FLD)
QCVM_OPCODE(LOAD_FNC)
QCVM_OPCODE(ADDRESS)
QCVM_OPCODE(STORE_F)
QCVM_OPCODE(STORE_V)
QCVM_OPCODE(STORE_S)
QCVM_OPCODE(STORE_ENT)
QCVM_OPCODE(STORE_FLD)
QCVM_OPCODE(STORE_FNC)
QCVM_OPCODE(STOREP_F)
QCVM_OPCODE(STOREP_V)
QCVM_OPCODE(STOREP_S)
QCVM_OPCODE(STOREP_ENT)
QCVM_OPCODE(STOREP_FLD)
QCVM_OPCODE(STOREP_FNC)
QCVM_OPCODE(RETURN)
QCVM_OPCODE(NOT_F)
QCVM_OPCODE(NOT_V)
QCVM_OPCODE(NOT_S)
QCVM_OPCODE(NOT_ENT)
QCVM_OPCODE(NOT_FNC)
QCVM_OPCODE(IF)
QCVM_OPCODE(IFNOT)
QCVM_OPCODE(CALL0)
QCVM_OPCODE(CALL1)
QCVM_OPCODE(CALL2)
QCVM_OPCODE(CALL3)
QCVM_OPCODE(CALL4)
QCVM_OPCODE(CALL5)
QCVM_OPCODE(CALL6)
QCVM_OPCODE(CALL7)
QCVM_OPCODE(CALL8)
QCVM_OPCODE(STATE)
QCVM_OPCODE(GOTO)
QCVM_OPCODE(AND_F)
QCVM_OPCODE(OR_F)
QCVM_OPCODE(BITAND_F)
QCVM_OPCODE(BITOR_F)

/* 
 * hexen 2 opcodes 
 */

QCVM_OPCODE(MULSTORE_F)
QCVM_OPCODE(MULSTORE_V)
QCVM_OPCODE(MULSTOREP_F)
QCVM_OPCODE(MULSTOREP_V)
QCVM_OPCODE(DIVSTORE_F)
QCVM_OPCODE(DIVSTOREP_F)
QCVM_OPCODE(ADDSTORE_F)
QCVM_OPCODE(ADDSTORE_V)
QCVM_OPCODE(ADDSTOREP_F)
QCVM_OPCODE(ADDSTOREP_V)
QCVM_OPCODE(SUBSTORE_F)
QCVM_OPCODE(SUBSTORE_V)
QCVM_OPCODE(SUBSTOREP_F)
QCVM_OPCODE(SUBSTOREP_V)
QCVM_OPCODE(FETCH_GBL_F)
QCVM_OPCODE(FETCH_GBL_V)
QCVM_OPCODE(FETCH_GBL_S)
QCVM_OPCODE(FETCH_GBL_E)
QCVM_OPCODE(FETCH_GBL_FNC)
QCVM_OPCODE(CSTATE)
QCVM_OPCODE(CWSTATE)
QCVM_OPCODE(THINKTIME)
QCVM_OPCODE(BITSET)
QCVM_OPCODE(BITSETP)
QCVM_OPCODE(BITCLR)
QCVM_OPCODE(BITCLRP)
QCVM_OPCODE(RAND0)
QCVM_OPCODE(RAND1)
QCVM_OPCODE(RAND2)
QCVM_OPCODE(RANDV0)
QCVM_OPCODE(RANDV1)
QCVM_OPCODE(RANDV2)
QCVM_OPCODE(SWITCH_F)
QCVM_OPCODE(SWITCH_V)
QCVM_OPCODE(SWITCH_S)
QCVM_OPCODE(SWITCH_E)
QCVM_OPCODE(SWITCH_FNC)
QCVM_OPCODE(CASE)
QCVM_OPCODE(CASERANGE)

/*
 * extended opcodes (fte) darkplaces) etc)
 */

/* hexen 2 calling conventions */
QCVM_OPCODE(CALL1H)
QCVM_OPCODE(CALL2H)
QCVM_OPCODE(CALL3H)
QCVM_OPCODE(CALL4H)
QCVM_OPCODE(CALL5H)
QCVM_OPCODE(CALL6H)
QCVM_OPCODE(CALL7H)
QCVM_OPCODE(CALL8H)

/* integer ops */
QCVM_OPCODE(STORE_I)
QCVM_OPCODE(STORE_IF)
QCVM_OPCODE(STORE_FI)
QCVM_OPCODE(ADD_I)
QCVM_OPCODE(ADD_FI)
QCVM_OPCODE(ADD_IF)
QCVM_OPCODE(SUB_I)
QCVM_OPCODE(SUB_FI)
QCVM_OPCODE(SUB_IF)
QCVM_OPCODE(CONV_ITOF)
QCVM_OPCODE(CONV_FTOI)
QCVM_OPCODE(LOADP_ITOF)
QCVM_OPCODE(LOADP_FTOI)
QCVM_OPCODE(LOAD_I)
QCVM_OPCODE(STOREP_I)
QCVM_OPCODE(STOREP_IF)
QCVM_OPCODE(STOREP_FI)
QCVM_OPCODE(BITAND_I)
QCVM_OPCODE(BITOR_I)
QCVM_OPCODE(MUL_I)
QCVM_OPCODE(DIV_I)
QCVM_OPCODE(EQ_I)
QCVM_OPCODE(NE_I)
QCVM_OPCODE(IFNOT_S)
QCVM_OPCODE(IF_S)
QCVM_OPCODE(NOT_I)
QCVM_OPCODE(DIV_VF)
QCVM_OPCODE(BITXOR_I)
QCVM_OPCODE(RSHIFT_I)
QCVM_OPCODE(LSHIFT_I)
QCVM_OPCODE(GLOBALADDRESS)
QCVM_OPCODE(ADD_PIW)
QCVM_OPCODE(LOADA_F)
QCVM_OPCODE(LOADA_V)
QCVM_OPCODE(LOADA_S)
QCVM_OPCODE(LOADA_ENT)
QCVM_OPCODE(LOADA_FLD)
QCVM_OPCODE(LOADA_FNC)
QCVM_OPCODE(LOADA_I)
QCVM_OPCODE(STORE_P)
QCVM_OPCODE(LOAD_P)
QCVM_OPCODE(LOADP_F)
QCVM_OPCODE(LOADP_V)
QCVM_OPCODE(LOADP_S)
QCVM_OPCODE(LOADP_ENT)
QCVM_OPCODE(LOADP_FLD)
QCVM_OPCODE(LOADP_FNC)
QCVM_OPCODE(LOADP_I)
QCVM_OPCODE(LE_I)
QCVM_OPCODE(GE_I)
QCVM_OPCODE(LT_I)
QCVM_OPCODE(GT_I)
QCVM_OPCODE(LE_IF)
QCVM_OPCODE(GE_IF)
QCVM_OPCODE(LT_IF)
QCVM_OPCODE(GT_IF)
QCVM_OPCODE(LE_FI)
QCVM_OPCODE(GE_FI)
QCVM_OPCODE(LT_FI)
QCVM_OPCODE(GT_FI)
QCVM_OPCODE(EQ_IF)
QCVM_OPCODE(EQ_FI)

/* string manipulation */
QCVM_OPCODE(ADD_SF)
QCVM_OPCODE(SUB_S)
QCVM_OPCODE(STOREP_C)
QCVM_OPCODE(LOADP_C)

/* general */
QCVM_OPCODE(MUL_IF)
QCVM_OPCODE(MUL_FI)
QCVM_OPCODE(MUL_VI)
QCVM_OPCODE(MUL_IV)
QCVM_OPCODE(DIV_IF)
QCVM_OPCODE(DIV_FI)
QCVM_OPCODE(BITAND_IF)
QCVM_OPCODE(BITOR_IF)
QCVM_OPCODE(BITAND_FI)
QCVM_OPCODE(BITOR_FI)
QCVM_OPCODE(AND_I)
QCVM_OPCODE(OR_I)
QCVM_OPCODE(AND_IF)
QCVM_OPCODE(OR_IF)
QCVM_OPCODE(AND_FI)
QCVM_OPCODE(OR_FI)
QCVM_OPCODE(NE_IF)
QCVM_OPCODE(NE_FI)

/* array access */
QCVM_OPCODE(GSTOREP_I)
QCVM_OPCODE(GSTOREP_F)
QCVM_OPCODE(GSTOREP_ENT)
QCVM_OPCODE(GSTOREP_FLD)
QCVM_OPCODE(GSTOREP_S)
QCVM_OPCODE(GSTOREP_FNC)
QCVM_OPCODE(GSTOREP_V)
QCVM_OPCODE(GADDRESS)
QCVM_OPCODE(GLOAD_I)
QCVM_OPCODE(GLOAD_F)
QCVM_OPCODE(GLOAD_FLD)
QCVM_OPCODE(GLOAD_ENT)
QCVM_OPCODE(GLOAD_S)
QCVM_OPCODE(GLOAD_FNC)

/* general */
QCVM_OPCODE(BOUNDCHECK)
QCVM_OPCODE(UNUSED)
QCVM_OPCODE(PUSH)
QCVM_OPCODE(POP)
QCVM_OPCODE(SWITCH_I)
QCVM_OPCODE(GLOAD_V)
QCVM_OPCODE(IF_F)
QCVM_OPCODE(IFNOT_F)
QCVM_OPCODE(STOREF_V)
QCVM_OPCODE(STOREF_F)
QCVM_OPCODE(STOREF_S)
QCVM_OPCODE(STOREF_I)
QCVM_OPCODE(STOREP_B)
QCVM_OPCODE(LOADP_B)

/* 32-bit ints */
QCVM_OPCODE(LE_U)
QCVM_OPCODE(LT_U)
QCVM_OPCODE(DIV_U)
QCVM_OPCODE(RSHIFT_U)

/* 64-bit types */
QCVM_OPCODE(ADD_I64)
QCVM_OPCODE(SUB_I64)
QCVM_OPCODE(MUL_I64)
QCVM_OPCODE(DIV_I64)
QCVM_OPCODE(BITAND_I64)
QCVM_OPCODE(BITOR_I64)
QCVM_OPCODE(BITXOR_I64)
QCVM_OPCODE(LSHIFT_I64I)
QCVM_OPCODE(RSHIFT_I64I)
QCVM_OPCODE(LE_I64)
QCVM_OPCODE(LT_I64)
QCVM_OPCODE(EQ_I64)
QCVM_OPCODE(NE_I64)
QCVM_OPCODE(LE_U64)
QCVM_OPCODE(LT_U64)
QCVM_OPCODE(DIV_U64)
QCVM_OPCODE(RSHIFT_U64I)
QCVM_OPCODE(STORE_I64)
QCVM_OPCODE(STOREP_I64)
QCVM_OPCODE(STOREF_I64)
QCVM_OPCODE(LOAD_I64)
QCVM_OPCODE(LOADA_I64)
QCVM_OPCODE(LOADP_I64)
QCVM_OPCODE(CONV_UI64)
QCVM_OPCODE(CONV_II64)
QCVM_OPCODE(CONV_I64I)
QCVM_OPCODE(CONV_FD)
QCVM_OPCODE(CONV_DF)
QCVM_OPCODE(CONV_I64F)
QCVM_OPCODE(CONV_FI64)
QCVM_OPCODE(CONV_I64D)
QCVM_OPCODE(CONV_DI64)

/* doubles */
QCVM_OPCODE(ADD_D)
QCVM_OPCODE(SUB_D)
QCVM_OPCODE(MUL_D)
QCVM_OPCODE(DIV_D)
QCVM_OPCODE(LE_D)
QCVM_OPCODE(LT_D)
QCVM_OPCODE(EQ_D)
QCVM_OPCODE(NE_D)
