#!/usr/bin/env node

import Path from 'path';

// Since JS does not allow seeding of PRNGs, traces for all except
// test are non-reproducible

/***************************** Program Trace ***************************/

const MAX_NEAR_RANGE = 1024;
const MAX_FAR_RANGE = 1024*64;
const MAX_OP_LEN = 15;


function* program(opts) {
  const { len, 'lo-addr': addrLo, mem: addrHi,
	  'call-prob': callProb, 'jump-prob': jumpProb, write } = opts;
  const rand = (lo, hi) => Math.trunc(lo + Math.random()*(hi - lo));
  const range = (addrHi - addrLo);
  let nearRange = Math.trunc(range / 1000);
  if (nearRange > MAX_NEAR_RANGE) nearRange = MAX_NEAR_RANGE;
  const logNear = Math.trunc(Math.log10(nearRange));
  let farRange = Math.trunc(range / 100);
  if (farRange > MAX_FAR_RANGE) farRange = MAX_FAR_RANGE;
  const stack = [];

  const nextAddr = (addr) => {
    let toss = Math.random();
    if (toss < jumpProb) {
      //use log's to make close jumps more likely than far jumps
      const r = Math.random()*(logNear - 2) + 2;
      let offset = rand(10**(r - 1), 10**r);
      if (offset > nearRange) offset = nearRange;
      return (Math.random() < 0.3) ? addr + offset : addr - offset;
    }
    toss = Math.random();
    if (toss < callProb) {
      if (stack.length === 0 || Math.random() < 0.5) {
	stack.push(addr + 1 + rand(0, MAX_OP_LEN));
	const callOffset = rand(0, farRange);
	return (Math.random() < 0.5) ? addr + callOffset : addr - callOffset;
      }
      else {
	return stack.pop();
      }
    }
    return addr + 1 + rand(0, MAX_OP_LEN);
  }
  let addr = Math.trunc(addrLo + range/2);
  for (let i = 0; i < len; i++) {
    addr = nextAddr(addr);
    if (addr < addrLo || addr > addrHi) addr = addrLo + rand(0, 20);
    yield { addr, access: 'r' };
  }
}

/***************************** Random Trace ****************************/

function* random(opts) {
  const { len, 'lo-addr': addrLo, mem: addrHi, write } = opts;
  const addrGranularity = 4;
  const range = (addrHi - addrLo)/addrGranularity;
  for (let i = 0; i < len; i++) {
    const addr = Math.trunc(Math.random() * range) * addrGranularity + addrLo;
    const access = Math.random() < write ? 'w' : 'r';
    yield { addr, access };
  }
}

/***************************** Stride Trace ****************************/

function* stride(opts) {
  const { len, 'lo-addr': addrLo, mem: addrHi, stride, write } = opts;
  let elementOffset = 0;
  let addr = addrLo;
  for (let i = 0; i < len; i++) {
    const access = Math.random() < write ? 'w' : 'r';
    yield { addr, access };
    addr += stride;
    if (addr > addrHi) addr = addrLo;
  }
}

/****************************** Test Trace *****************************/

function* test(opts) {
  const { len, 'lo-addr': addrLo, mem: addrHi, blk, set, ways } = opts;
  const nSets = 1<<set;
  const maxOffset1 = 1 << blk;
  let maxSet1 = 1 << set;
  let n = 0;
  let offset = 0;
  for (let s = 0; s < maxSet1; s+= 2) {
    for (let tag = 0; tag < 2*ways; tag++) {
      for (let w = 0; w < ways; w++) {
	let s1 = (offset % 2) == 0 ? s : s + 1;
	const addr = (tag << (set + blk)) + (s1 << blk) + offset;
	const access = (offset%3 === 0) ? 'w' : 'r';
	yield { addr, access };
	if (++n >= len) break;
	if (++offset >= maxOffset1) offset = 0;
      }
    }
  }
}


/************************** Command-Line Handling **********************/

const OPTIONS = {
  '': {
    descr: 'General Options',
    options: [
      { long: 'len',
	short: 'n',
	descr: '# of address traces',
	defaultVal: 100,
      },
      { long: 'lo-addr',
	short: 'l',
	defaultVal: '0x1000',
	format: 'hex',
	descr: 'low base address in hex',
      },
      { long: 'mem',
	short: 'm',
	defaultVal: '64k',
	format: 'memSize',
	descr: 'total memory size; optional suffix k, m, g',
      },
    ],
  },

  stride: {
    descr: 'generate trace with specific stride through memory',
    options: [
      { long: 'stride',
	short: 'r',
	defaultVal: 4,
	descr: '# of bytes over which to stride',
      },
      { long: 'write',
	short: 'w',
	defaultVal: 0.4,
	format: 'probability',
	descr: 'probability of a write access',
      },
    ],
  },

  program: {
    descr: 'generate read-only typical program trace',
    options: [
      { long: 'jump-prob',
	short: 'j',
	defaultVal: 0.1,
	format: 'probability',
	descr: 'probability of taking intra-function jump',
      },
      { long: 'call-prob',
	short: 'c',
	defaultVal: 0.04,
	format: 'probability',
	descr: 'probability of call/return',
      },
    ],
  },

  
  random: {
    descr: 'generate random trace',
    options: [
      { long: 'write',
	short: 'w',
	defaultVal: 0.2,
	format: 'probability',
	descr: 'probability of a write access',
      },
    ],
  },
  
  test: {
    descr: 'generate test trace for systematically testing cache',
    options: [
      { long: 'blk',
	short: 'b',
	defaultVal: 4,
	descr: '# of bits in block offset',
      },
      { long: 'set',
	short: 's',
	defaultVal: 4,
	descr: '# of bits in set index',
      },
      { long: 'ways',
	short: 'E',
	defaultVal: 2,
	descr: '# of ways in set',
      },
    ],
  },
  
};

function help() {
  const W = 18;
  const optHelp = opt => {
    const head = `--${opt.long} | -${opt.short}:`.padEnd(W);
    return `    ${head}${opt.descr}; default ${opt.defaultVal}\n`;
  }
  let text = '';
  text += `usage: ${Path.basename(process.argv[1])} MODE OPTIONS...\n`;
  const modes = Object.keys(OPTIONS).filter(m => m.length > 0);
  text += `MODE is either ${modes.join(' or ')}.\n`;
  Object.keys(OPTIONS).forEach(k => {
    if (k) {
      text += `  MODE ${k}: ${OPTIONS[k].descr}; Options:\n`;
    }
    else {
      text += '  General Options:\n';
    }
    OPTIONS[k].options.forEach(opt => text += optHelp(opt));
  });
  return text;
}

function usage() {
  console.log(help());
  process.exit(1);
}

function optFormat(optName, optValStr, optFormat='int') {
  const str = optValStr.trim();
  let val;
  const err = console.err;
  switch (optFormat) {
    case 'hex': 
      val = parseInt(optValStr, 16);
      break;
    case 'memSize': {
      const m = optValStr.match(/(^\d+)([kmg]?)$/);
      if (!m) {
	err(`bad memSize "${optValStr}" for option ${optName}; ` +
	    'must be a number with an optional suffix of k, m or b');
	process.exit(1);
      }
      val = Number(m[1]);
      if (m[2]) val *= 1024 ** ('kmg'.indexOf(m[2]) + 1);
      break;
    }
    case 'probability': 
      val = Number(optValStr);
      if (!(0 <= val) || !(val <= 1)) {
	err(`option ${optName} must specify a probability in [0, 1]`);
	process.exit(1);
      }
      break;
    case 'int':
      val = Number(optValStr);
      if (/\./.test(optValStr) || isNaN(val) || val < 0) {
	err(`option ${optName} must specify a non-negative integer`);
	process.exit(1);
      }
      break;
    default:
      console.assert(false, `unhandled format ${optFormat}`);
      break;
  }
  return val;
}

function getOptions(args) {
  if (args.length === 0) usage();
  const mode = args[0];
  if (!OPTIONS[mode]) usage();
  const optionInfos = [ OPTIONS[''].options, OPTIONS[mode].options, ].flat();
  const findOptionInfo = optArg => {
    const isLong = optArg.startsWith('--');
    const optName = isLong ? optArg.slice(2) : optArg[1];
    const key = isLong ? 'long' : 'short';
    return optionInfos.find(info => info[key] == optName);
  };
  const err = console.error;
  const options = {};
  for (let i = 1; i < args.length; i++) {
    const arg = args[i];
    if (!arg.startsWith('-')) { err(`"${arg}" is not an option`); usage(); }
    const optInfo = findOptionInfo(arg);
    if (!optInfo) { err(`unknown option arg "${arg}"`); usage(); }
    const optVal =
      (arg[1] !== '-' && arg.length > 2) ? arg.slice(2) : args[++i];
    options[optInfo.long] = optVal;
  }
  for (const optInfo of optionInfos) {
    const optName = optInfo.long;
    const optValStr = options[optName] ?? String(optInfo.defaultVal);
    options[optInfo.long] = optFormat(optName, optValStr, optInfo.format);
  }
  options.mode = mode;
  return options;
}


const MODE_FNS = { program, random, stride, test, };

function go(args) {
  const opts = getOptions(args);
  const addrWidth = (Math.log2(opts.mem) + 3)/4;
  for (const trace of MODE_FNS[opts.mode](opts)) {
    const addr = trace.addr.toString(16).padStart(addrWidth, '0');
    console.log(`0x${addr} ${trace.access}`);
  }
}

go(process.argv.slice(2));
