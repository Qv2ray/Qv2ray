# Contributing Qv2ray

Let's make Qv2ray great

## Branches

- ***Do not*** fix/push the code in/into the `master` branch.
- When submitting a small bugfix (code change less than 10 lines), please use the `version-v*` branch.
  - `*` is the target major version.
- When submitting a larger bugfix, using `dev` is required since the stability of your patch needs to be examined.
- When submitting a new feature, it's recommended to checkout a new branch, named `dev-**` 
  - Where the feature should be briefly explained by the branch name, e.g. `dev-plugin-system` or `dev-qr-code`
  
## Code style

- Always run `clang-format` before submitting your patch, this is to prevent git changes caused by the code formatter.
- A `.clang-format` spec file has been placed at the root directory of the project.
- There's no strict limit of the naming style, but typically:
  - All function names, class names are in UpperCamelCase
  - All namespaces should be lowercase except `::Qv2ray`, where the corresponding file location should be the same as your namespace
  - All local variables, member variables should be in normal CamelCase
  - Unused variables should be wrapped with `Q_UNUSED`
  - Try to use forward declaration instead of including headers in another head file, except UI file.
  - Try not to modify the `.ui` files, if that is necessary, please note that you should also be capable fixing the git merge conflict if occurs.
  - For header files, use `hpp` suffix.
  
E.g.: A file named UpperCamelCase.cpp/hpp should be located at `./src/components/featuretype/featuresubtype/`

```c++
namespace Qv2ray::components::featuretype::featuresubtype {
  class UpperCamelCase : QObject {
      Q_OBJECT
    signals:
      void OK();
    public: 
      explicit UpperCamelCase(QObject *parent);
      void DoWork() {
          auto [data, status, isDataValid, hasErrorOccured] = CallSomethingElse();
          Q_UNUSED(data)
          status.clear();
          if(isDataValid && !hasErrorOccured) {
              emit OK();
          }
      }
      ~UpperCamelCase();
    private:
      UpperCamelCase *normalCamelCase;
      QWidget *myNewWidget;
  }
}
```
