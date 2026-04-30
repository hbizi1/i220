import { spawn } from 'node:child_process';
import readline from 'node:readline';

export default function execProg(execPath) {
  const child = spawn(execPath);
  const rl = readline.createInterface({
    input: child.stdout,
    terminal: false,
  });
  function getLine() {
    return new Promise((resolve, reject) => {
      rl.on('line', line => resolve(line));
    });
  }
  function putLine(line) {
    if (!line.endsWith('\n')) line += '\n';
    child.stdin.write(line);
  }
  function close() {
    child.kill();
    return new Promise((resolve, reject) => {
      child.on('close', code => {
	console.error(`child process "${execPath}" terminated ` +
		      `code ${code}`);
	resolve('closed');
      });
    });
  }
  return { getLine, putLine, close };
}
