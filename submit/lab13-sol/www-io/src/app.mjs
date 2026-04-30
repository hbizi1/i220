import express from 'express';

import Path from 'node:path';


export default function serve(execPath, exec, options={})
{
  const app = express();
  app.locals.base = options.base ?? '/api';
  const srcDir = Path.dirname(new URL(import.meta.url).pathname);
  app.locals.mainPage = `${srcDir}/files/${Path.basename(execPath)}.html`;
  app.locals.exec = exec;
  setupRoutes(app);
  return app;
}

function setupRoutes(app) {
  // const srcPath = new URL(import.meta.url).pathname;
  // const publicPath = Path.join(Path.dirname(srcPath), '../public');
  // app.use(express.static(publicPath));

  const { base } = app.locals;

  app.use(express.json());

  app.get('/', doMainPage(app));

  app.post(`${base}/post-line`, doPostLine(app));
  app.post(`${base}/out-in-line`, doOutInLine(app));
}


function doMainPage(app) {
  return async (req, res) => {
    res.sendFile(app.locals.mainPage);
  };
}

function doPostLine(app) {
  return async (req, res) => {
    try {
      const line = req.body.line;
      const exec = app.locals.exec;
      await exec.putLine(line);
      res.json({ok: true});
    }
    catch (err) {
      console.error(err);
    }
  };
}

function doOutInLine(app) {
  return async (req, res) => {
    try {
      const line = req.body.line;
      const exec = app.locals.exec;
      await exec.putLine(line);
      const line1 = await exec.getLine();
      res.json({line: line1});
    }
    catch (err) {
      console.error(err);
    }
  };
}
