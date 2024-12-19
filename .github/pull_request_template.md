<!-- The text within this markup is a comment, and is intended to provide
guidelines to open a Pull Request for the vxl repository. This text will not
be part of the Pull Request. -->

<!--
Start vxl commit messages with a standard prefix (and a space):

 * BUG: fix for runtime crash or incorrect result
 * COMP: compiler error or warning fix
 * DOC: documentation change
 * ENH: new functionality
 * PERF: performance improvement
 * STYLE: no logic impact (indentation, comments)
 * WIP: Work In Progress not ready for merge

Provide a short, meaningful message that describes the change you made.

When the PR is based on a single commit, the commit message is usually left as
the PR message.

A reference to a related issue or pull request
(https://help.github.com/articles/basic-writing-and-formatting-syntax/#referencing-issues-and-pull-requests)
in your repository. You can automatically close a related issues using keywords
(https://help.github.com/articles/closing-issues-using-keywords/)

@mentions (https://help.github.com/articles/basic-writing-and-formatting-syntax/#mentioning-people-and-teams)
of the person or team responsible for reviewing proposed changes. -->

## PR Checklist
<!-- Delete either [X] or :no_entry_sign: to indicate if the statement is true or false. -->

- <!-- [X] or :no_entry_sign: --> Makes breaking changes to the vxl/core/\* API that requires semantic versioning increase
- <!-- [X] or :no_entry_sign: --> Makes design changes to existing vxl/core\* API that requires semantic versioning increase
<!--
If either of the above two items is true,
    the vxl/CMakeLists.txt project VERSION needs to bumped to a higher version
    VERSION 2.0.2.0 # defines #VXL_VERSION{,MAJOR,MINOR,PATCH,TWEAK}
    Follow the conventions described at https://semver.org
-->
- <!-- [X] or :no_entry_sign: --> Makes changes to the contributed directory API DOES NOT require semantic versioning increase
- <!-- [X] or :no_entry_sign: --> Adds tests and baseline comparison (quantitative).
- <!-- [X] or :no_entry_sign: --> Adds Documentation.


<!-- **Thanks for contributing to vxl!** -->
