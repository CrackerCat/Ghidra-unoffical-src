/* ###
 * IP: GHIDRA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package help;

import java.net.URI;
import java.nio.file.Path;

/**
 * A class that represents the original location of an IMG tag along with its location 
 * resolution within the help system.
 */
public class ImageLocation {

	private Path sourceFile;
	private String imageSrc;

	private Path resolvedPath;
	private URI resolvedUri;
	private boolean isRemote;
	private boolean isRuntime;

	public static ImageLocation createLocalLocation(Path sourceFile, String imageSrc,
			URI resolvedUri, Path resolvedPath) {

		ImageLocation l = new ImageLocation(sourceFile, imageSrc);
		l.resolvedUri = resolvedUri;
		l.resolvedPath = resolvedPath;
		l.isRemote = false;
		l.isRuntime = false;
		return l;
	}

	public static ImageLocation createRuntimeLocation(Path sourceFile, String imageSrc,
			URI resolvedUri, Path resolvedPath) {

		ImageLocation l = new ImageLocation(sourceFile, imageSrc);
		l.resolvedUri = resolvedUri;
		l.resolvedPath = resolvedPath;
		l.isRemote = false;
		l.isRuntime = true;
		return l;
	}

	public static ImageLocation createInvalidRuntimeLocation(Path sourceFile, String imageSrc) {

		ImageLocation l = new ImageLocation(sourceFile, imageSrc);
		l.resolvedUri = null;
		l.resolvedPath = null;
		l.isRemote = false;
		l.isRuntime = true;
		return l;
	}

	public static ImageLocation createRemoteLocation(Path sourceFile, String imageSrc,
			URI resolvedUri) {

		ImageLocation l = new ImageLocation(sourceFile, imageSrc);
		l.resolvedUri = resolvedUri;
		l.resolvedPath = null;
		l.isRemote = true;
		l.isRuntime = false;
		return l;
	}

	private ImageLocation(Path sourceFile, String imageSrc) {
		this.sourceFile = sourceFile;
		this.imageSrc = imageSrc;
	}

	public Path getSourceFile() {
		return sourceFile;
	}

	public void setSourceFile(Path sourceFile) {
		this.sourceFile = sourceFile;
	}

	public String getImageSrc() {
		return imageSrc;
	}

	public void setImageSrc(String imageSrc) {
		this.imageSrc = imageSrc;
	}

	public Path getResolvedPath() {
		return resolvedPath;
	}

	public void setResolvedPath(Path resolvedPath) {
		this.resolvedPath = resolvedPath;
	}

	public URI getResolvedUri() {
		return resolvedUri;
	}

	public void setResolvedUri(URI resolvedUri) {
		this.resolvedUri = resolvedUri;
	}

	public boolean isRemote() {
		return isRemote;
	}

	public void setRemote(boolean isRemote) {
		this.isRemote = isRemote;
	}

	public boolean isRuntime() {
		return isRuntime;
	}

	public void setRuntime(boolean isRuntime) {
		this.isRuntime = isRuntime;
	}

	@Override
	public String toString() {
		//@formatter:off
		return "{\n" + 
			"\tsource file: " + sourceFile + ",\n" +
			"\tsrc: " + imageSrc + ",\n" +
			"\turi: " + resolvedUri + ",\n" +
			"\tpath: " + resolvedPath + ",\n" +
			"\tis runtime: " + isRuntime + ",\n" +
			"\tis remote: " + isRemote + "\n" +
		"}";
		//@formatter:on
	}
}
