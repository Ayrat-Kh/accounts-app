module.exports = {
  ignorePatterns: ['**/*.css', '**/*.sh'],
  env: {
    browser: true,
    es2021: true,
  },
  extends: [
    'eslint:recommended',
    'plugin:@typescript-eslint/recommended',
    'plugin:react/recommended',
    'plugin:import/errors',
    'plugin:import/warnings',
    'plugin:import/typescript',
  ],
  // overrides: [
  //   {
  //     env: {
  //       node: true,
  //     },
  //     files: ['.eslintrc.{js,cjs}'],
  //     parserOptions: {
  //       sourceType: 'script',
  //     },
  //   },
  // ],
  parser: '@typescript-eslint/parser',
  parserOptions: {
    ecmaVersion: 'latest',
    sourceType: 'module',
    project: __dirname + '/tsconfig.json',
  },
  settings: {
    'import/extensions': ['.js', '.jsx', '.ts', '.tsx'],
    'import/parsers': {
      '@typescript-eslint/parser': ['.ts', '.tsx'],
    },
    'import/resolver': {
      typescript: {
        project: __dirname + '/tsconfig.json',
      }, // this loads <rootdir>/tsconfig.json to eslint
    },
  },
  plugins: ['@typescript-eslint', 'react', 'import'],
  rules: {
    'react/react-in-jsx-scope': 'off',
    'react/prop-types': 'off',
    'import/namespace': 'off',
    'import/no-cycle': [
      'error',
      {
        maxDepth: 1,
        ignoreExternal: true,
      },
    ],
  },
  ignorePatterns: ['metro.config.js', 'babel.config.js', 'tailwind.config.js'],
};
