describe('window.eparakstshwcrypto', function(){
	var expect = chai.expect;
	it('should exist after script inclusion', function () {
		expect(window.eparakstshwcrypto).to.be.a('object'); 
	});
	it('should not have a version property', function() {
		expect(window.eparakstshwcrypto).not.have.a.property('version');
	});
	it('should not have a getVersion() method', function() {
		expect(window.eparakstshwcrypto).itself.not.to.respondTo('getVersion');
	});
	it('should have a getCertificate() method', function() {
		expect(window.eparakstshwcrypto).itself.to.respondTo('getCertificate');
	});
	it('should have a sign() method', function() {
		expect(window.eparakstshwcrypto).itself.to.respondTo('sign');
	});

	describe('backend selection', function() {
		it('should have use() method', function() {
			expect(window.eparakstshwcrypto).itself.to.respondTo('use');
		});
	});

	describe('debugging capabilities', function(){
		it('should have debug() method', function() {
			expect(window.eparakstshwcrypto).itself.to.respondTo('debug');
		});
		it('should always succeed', function() {
			return window.eparakstshwcrypto.debug().should.eventually.be.a('string');
		});
		it('should always contain "eparakstshwcrypto"', function() {
			return window.eparakstshwcrypto.debug().should.eventually.contain('eparakstshwcrypto');
		});
	});

	describe('.getCertificate()', function(){
		it('should be rejected without backend', function(){
			return window.eparakstshwcrypto.getCertificate({}).should.be.rejectedWith(Error, window.eparakstshwcrypto.NO_IMPLEMENTATION);
		});
	});
	describe('.sign()', function(){
		it('should be rejected without backend', function(){
			return window.eparakstshwcrypto.sign({}, {}, {}).should.be.rejectedWith(Error, window.eparakstshwcrypto.NO_IMPLEMENTATION);
		});
	});
}); 

