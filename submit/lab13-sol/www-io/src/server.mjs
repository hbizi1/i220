import execProg from './exec-prog.mjs';
import serve from './app.mjs';

import fs from 'node:fs';
import https from 'node:https';
import Path from 'node:path';

async function go(args) {
  if (args.length !== 1 && args.length !== 2) {
    panic(`usage: ${Path.basename(process.argv[1])} C_EXEC_PATH [PORT]`);
  }
  try {
    const execPath = args[0];
    const srcPath = new URL(import.meta.url).pathname;
    const configPath = Path.join(Path.dirname(srcPath), '../config.mjs');
    const config = (await import(configPath)).default;
    const port = (args.length === 2) ? Number(args[1]) : config.port;
    if (port < 1024) panic(`bad port ${port}: must be >= 1024`);
    const exec = execProg(execPath);
    const app = serve(execPath, exec);
    const serverOpts = {
      key: fs.readFileSync(config.keyPath),
      cert: fs.readFileSync(config.certPath),
    };
    const server = https.createServer(serverOpts, app)
      .listen(port, function() {
	console.log(`listening on port ${port}`);
      });
  }
  catch (err) {
    console.error(err);
    process.exit(1);
  }
}

function panic(...args) {
  console.error(...args);
  process.exit(1);
}

await go(process.argv.slice(2));
