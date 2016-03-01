# Vittf Is a Tiny Test Framework

## Sources
* [TinyTest](https://github.com/joewalnes/tinytest)
* [Moulitest](https://github.com/yyang42/moulitest)

## Example
``` c
void	test_name1(void)
{
	v_assert(1 == 1);
	v_assert_str("Hello", "World!");

	v_test_success("Name1");
}

void	suite_name(void)
{
	test_name1();

	v_suite_success("Filename");
}

int		main(void)
{
	suite_name();

	v_full_success("Program");
	return (0);
}
```
