# How we work on SoundAlike (GitHub + AI)

A quick guide for both of us. If you get lost, message Daniel.

## One-time setup

1. Make a GitHub account and accept the invite to `danielLinde12/SoundALike`
   (check your email or github.com/notifications).
2. Install **GitHub Desktop** (desktop.github.com) and sign in with **your own** account.
   Commits are credited to whoever is signed in, and our grade checks that both of us commit.
3. In GitHub Desktop: **File → Clone repository → SoundALike → Clone**.
   This makes a SoundALike folder on your computer linked to GitHub.
4. (If using Claude Code) Install it, open a terminal in the SoundALike folder, and run `claude`.
   It automatically reads `CLAUDE.md`, which explains the whole project and its rules.

## Words you'll see

| Word | Meaning |
| --- | --- |
| Repository (repo) | The project folder, with its full history. |
| Commit | A save point: a snapshot of some changes plus a short message. |
| Branch | A separate line of work, so your changes don't touch `main` until they're ready. |
| Push / Publish | Upload your commits from your computer to GitHub. |
| Pull | Download the other person's commits from GitHub to your computer. |
| Pull request (PR) | A request to merge your branch into `main`; the other person can review it. |
| Merge | Combine a branch into `main`. |

## Every time you work on something

1. **Get the latest code.** Switch to `main` (**Current branch → main**), then **Fetch origin → Pull origin**.
2. **Make a branch** for the task: **Current branch → New branch**, with a short name like `data-cleaning`.
3. **Do the work** (with AI help if you want). Ask the AI to explain anything you don't understand:
   we have to explain every line at the showcase.
4. **Commit in small pieces.** In the **Changes** tab, check only the files for one change, write a short
   summary (e.g., "Drop rows with missing values"), and click **Commit**. Several small commits beat one huge one.
5. **Push:** click **Publish branch** the first time, then **Push origin** after later commits.
6. **Open a pull request:** **Branch → Create Pull Request**, then **Create pull request** on GitHub.
   Tell the other person so they can take a quick look.
7. **Merge:** on the PR page, **Merge pull request → Confirm merge**, then **Delete branch**.
8. **Update your computer:** switch back to `main` and **Pull origin**.

## Rules of thumb

- Commit at least once a week; the history is part of our grade.
- Never commit junk files (`.DS_Store`, `.swp`, big downloads). If you're unsure about a file, ask.
- The k-d tree and brute-force search must be our own C++. Python is fine for data cleaning,
  plots, and the interface.
- When AI helps with something notable, add a line to `AI_USAGE.md`, especially anything it got wrong
  that we had to fix. The report needs this.
- Don't work on the same file at the same time as the other person without telling them.
  If GitHub Desktop says there's a **merge conflict**, stop and message each other.
