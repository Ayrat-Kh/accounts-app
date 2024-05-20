import { getInputNumberValue } from './utils';

describe('getInputNumberValue', () => {
  test('should return new valid value #1', () => {
    expect(getInputNumberValue('123.', '123')).toEqual({
      value: 123,
      rawString: '123.',
    });
  });

  test('should return new valid value #2', () => {
    expect(getInputNumberValue('123.3', '123.')).toEqual({
      value: 123.3,
      rawString: '123.3',
    });
  });

  test('should return new valid value #3', () => {
    expect(getInputNumberValue('123,3', '123,')).toEqual({
      value: 123.3,
      rawString: '123,3',
    });
  });

  test('should return previous valid value #1', () => {
    expect(getInputNumberValue('123..', '123.')).toEqual({
      value: 123,
      rawString: '123.',
    });
  });

  test('should return previous valid value #2', () => {
    expect(getInputNumberValue('12a', '12')).toEqual({
      value: 12,
      rawString: '12',
    });
  });
});
